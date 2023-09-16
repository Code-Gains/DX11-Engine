#include "WindowsXpPipesSimulation.hpp"

WindowsXpPipesSimulation::WindowsXpPipesSimulation(const WRL::ComPtr<ID3D11Device>& device, const Int3& dimensions, const float simulationSpeed):
_device(device),
_dimensions(dimensions),
_simulationSpeed(simulationSpeed)
{
    Initialize(device.Get());
    Reset(dimensions, simulationSpeed);
}

WindowsXpPipesSimulation::~WindowsXpPipesSimulation() {}

bool WindowsXpPipesSimulation::Initialize(ID3D11Device* device)
{
    auto cylinder = Cylinder(DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0.5f, 1.0f, 0.5f), false);
    std::vector<VertexPositionNormalUv> cylinderVertices = cylinder.GetVertices();
    std::vector<UINT> cylinderIndices = cylinder.GetIndices();
    _instanceRenderer.InitializeInstancePool<VertexPositionNormalUv>(device, 0, cylinderVertices, cylinderIndices);

    auto sphere = Sphere(DirectX::XMFLOAT3( 0, 0, 0 ));
    std::vector<VertexPositionNormalUv> sphereVertices = sphere.GetVertices();
    std::vector<UINT> sphereIndices = sphere.GetIndices();
    _instanceRenderer.InitializeInstancePool<VertexPositionNormalUv>(device, 1, sphereVertices, sphereIndices);

    return true;
}

void WindowsXpPipesSimulation::Reset(const Int3& dimensions, float simulationSpeed)
{
    _timeUntilNextSegment = 1 / simulationSpeed;
    _currentDirection = Direction::PositiveY;
    _currentPosition = Int3(0, 0, 0);

    _grid.clear();
    _grid.resize(dimensions.x);
    for (int x = 0; x < _dimensions.x; ++x)
    {
        _grid[x].clear();
        _grid[x].resize(dimensions.y);
        for (int y = 0; y < _dimensions.y; ++y)
        {
            _grid[x][y].clear();
            _grid[x][y].resize(dimensions.z);
            for (int z = 0; z < _dimensions.z; ++z)
            {
                GridCell& gridCell = _grid[x][y][z];
                gridCell.type = GridCell::EMPTY;
                gridCell.instanceIndex = -1;
                gridCell.modelMatrixIndex = -1;
                gridCell.poolKey = GridCell::INVALID;
                
            }
        }
    }

    _pipeTransforms.clear();
    _instanceRenderer.RemoveAllInstances();
    _straightCount = 0;
    _cornerCount = 0;

    CreatePipeAtCell(_currentPosition, _currentDirection, GridCell::PIPE_STRAIGHT);
    _timeUntilNextSegment = 1 / simulationSpeed;
    _currentPosition = GetNextCell(_currentPosition, _currentDirection);


    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> lengthShortest(3, 5);
    _shortestStraight = lengthShortest(gen);

    std::uniform_int_distribution<> lengthLongest(_shortestStraight, 25);
    _longestStraight = lengthLongest(gen);

    std::uniform_real_distribution<> disRatio(1, 1.1);
    _turnProbabilityIncreaseRatio = disRatio(gen);

    ResetTurnProbability();
}

void WindowsXpPipesSimulation::Update(float deltaTime)
{
    _timeUntilNextSegment -= deltaTime;
    while (_timeUntilNextSegment < 0.0f)
    {
        Int3 previousCellPosition = GetPreviousCell(_currentPosition, _currentDirection);
        GridCell previousCell = _grid[previousCellPosition.x][previousCellPosition.y][previousCellPosition.z];
        Direction previousDirection = _currentDirection;

        std::vector<Direction> availableDirections = GetAvailableDirections();
        if (availableDirections.size() == 0)
        {
            Reset(_dimensions, _simulationSpeed);
            continue;
        }

        bool makeCorner = false;

        if (_currentStraightLength >= _shortestStraight)
        {
            Direction generatedDirection = GenerateDirection(availableDirections, _currentDirection, _turnProbability);
            makeCorner = generatedDirection != _currentDirection;
            if (makeCorner)
            {
                _currentDirection = generatedDirection;
                ResetTurnProbability();
            }
            else
                _turnProbability *= _turnProbabilityIncreaseRatio;
        }
        bool currentDirectionIsBlocked = std::find(availableDirections.begin(), availableDirections.end(), _currentDirection) == availableDirections.end();
        if (makeCorner || currentDirectionIsBlocked) {
            ExtendCellPipe(previousCell, previousDirection, 0.1);
            if (!makeCorner)
                _currentDirection = GetNextDirection(availableDirections);
            CreatePipeAtCell(_currentPosition, _currentDirection, GridCell::PIPE_CORNER);
            _currentPosition = GetNextCell(_currentPosition, _currentDirection);
            _currentStraightLength = 0;
            _timeUntilNextSegment = 1 / _simulationSpeed;
            return;
        }

        CreatePipeAtCell(_currentPosition, _currentDirection, GridCell::PIPE_STRAIGHT);

        if (previousCell.type == GridCell::PIPE_CORNER)
        {
            GridCell currentCell = GetCell(_currentPosition);
            ExtendCellPipe(currentCell, GetOppositeDirection(_currentDirection), 0.1);
        }
        _currentPosition = GetNextCell(_currentPosition, _currentDirection);
        _currentStraightLength += 1;
        _timeUntilNextSegment += 1 / _simulationSpeed;
    }
}

void WindowsXpPipesSimulation::Render(ID3D11DeviceContext* deviceContext, ID3D11Buffer* perObjectConstantBuffer, ID3D11Buffer* instanceConstantBuffer)
{
    _instanceRenderer.RenderInstances<VertexPositionNormalUv>(deviceContext, perObjectConstantBuffer, instanceConstantBuffer);
}

int WindowsXpPipesSimulation::GetOwnershipCount() const
{
    return _straightCount + _cornerCount;
}

GridCell WindowsXpPipesSimulation::GetCell(const Int3& position) const {
    return _grid[position.x][position.y][position.z];
}

Int3 WindowsXpPipesSimulation::GetNextCell(const Int3& currentPosition, const Direction currentDirection) const
{
    Int3 nextCell = currentPosition;

    switch (_currentDirection)
    {
    case Direction::PositiveX:
        nextCell.x += 1;
        break;
    case Direction::NegativeX:
        nextCell.x -= 1;
        break;
    case Direction::PositiveY:
        nextCell.y += 1;
        break;
    case Direction::NegativeY:
        nextCell.y -= 1;
        break;
    case Direction::PositiveZ:
        nextCell.z += 1;
        break;
    case Direction::NegativeZ:
        nextCell.z -= 1;
        break;
    default:
        break;
    }

    if (nextCell.x < 0 || nextCell.y < 0 || nextCell.z < 0
        || nextCell.x >= _dimensions.x || nextCell.y >= _dimensions.y || nextCell.z >= _dimensions.z)
    {
         throw std::runtime_error("Simulation is over. No available cells!");
    }

    if (_grid[nextCell.x][nextCell.y][nextCell.z].type != GridCell::EMPTY)
    {
        throw std::runtime_error("Next cell is already occupied!");
    }

    return nextCell;
}

Int3 WindowsXpPipesSimulation::GetPreviousCell(const Int3& currentPosition, const Direction direction) const
{
    Int3 previousCell = currentPosition;

    switch (direction) {
        case Direction::PositiveX:
            previousCell.x -= 1;
            break;
        case Direction::NegativeX:
            previousCell.x += 1;
            break;
        case Direction::PositiveY:
            previousCell.y -= 1;
            break;
        case Direction::NegativeY:
            previousCell.y += 1;
            break;
        case Direction::PositiveZ:
            previousCell.z -= 1;
            break;
        case Direction::NegativeZ:
            previousCell.z += 1;
            break;
        default:
            throw std::invalid_argument("Invalid direction");
    }

    if (previousCell.x < 0 || previousCell.y < 0 || previousCell.z < 0
        || previousCell.x >= _dimensions.x || previousCell.y >= _dimensions.y || previousCell.z >= _dimensions.z)
    {
        std::cout << "Empty!" << std::endl;
        throw std::runtime_error("Previous cell is out of bounds.");
    }

    if (_grid[previousCell.x][previousCell.y][previousCell.z].type == GridCell::EMPTY)
    {
        std::cout << "Out of bounds!" << std::endl;
        throw std::runtime_error("Previous cell does not exist.");
    }

    return previousCell;
}

WindowsXpPipesSimulation::Direction WindowsXpPipesSimulation::GetNextDirection(const std::vector<Direction>& availableDirections) const
{
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> distrib(0, availableDirections.size() - 1);

    int randomIndex = distrib(gen);

    return availableDirections[randomIndex];
}

std::vector<WindowsXpPipesSimulation::Direction> WindowsXpPipesSimulation::GetAvailableDirections() const
{
    std::vector<Direction> availableDirections;

    // PositiveX
    if (_currentPosition.x + 1 < _dimensions.x && _grid[_currentPosition.x + 1][_currentPosition.y][_currentPosition.z].type == GridCell::EMPTY)
        availableDirections.push_back(Direction::PositiveX);
    // NegativeX
    if (_currentPosition.x - 1 >= 0 && _grid[_currentPosition.x - 1][_currentPosition.y][_currentPosition.z].type == GridCell::EMPTY)
        availableDirections.push_back(Direction::NegativeX);
    // PositiveY
    if (_currentPosition.y + 1 < _dimensions.y && _grid[_currentPosition.x][_currentPosition.y + 1][_currentPosition.z].type == GridCell::EMPTY)
        availableDirections.push_back(Direction::PositiveY);
    // NegativeY
    if (_currentPosition.y - 1 >= 0 && _grid[_currentPosition.x][_currentPosition.y - 1][_currentPosition.z].type == GridCell::EMPTY)
        availableDirections.push_back(Direction::NegativeY);
    // PositiveZ
    if (_currentPosition.z + 1 < _dimensions.z && _grid[_currentPosition.x][_currentPosition.y][_currentPosition.z + 1].type == GridCell::EMPTY)
        availableDirections.push_back(Direction::PositiveZ);
    // NegativeZ
    if (_currentPosition.z - 1 >= 0 && _grid[_currentPosition.x][_currentPosition.y][_currentPosition.z - 1].type == GridCell::EMPTY)
        availableDirections.push_back(Direction::NegativeZ);

    return availableDirections;
}

DirectX::XMFLOAT3 WindowsXpPipesSimulation::GetCellWorldPosition(const Int3& cellPosition) const
{
    return DirectX::XMFLOAT3(cellPosition.x + transform.position.x, cellPosition.y + transform.position.y, cellPosition.z + transform.position.z);
}

DirectX::XMFLOAT3 WindowsXpPipesSimulation::GetRotationByDirection(const Direction direction) const
{
    using namespace Constants;
    DirectX::XMFLOAT3 rotation;

    switch (direction)
    {
        case Direction::PositiveX:
            rotation = DirectX::XMFLOAT3(0.0f, 0.0f, M_PI / 2);
            break;
        case Direction::NegativeX:
            rotation = DirectX::XMFLOAT3(0.0f, 0.0f, -M_PI / 2);
            break;
        case Direction::PositiveY:
            rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
            break;
        case Direction::NegativeY:
            rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
            break;
        case Direction::PositiveZ:
            rotation = DirectX::XMFLOAT3(-M_PI / 2, 0.0f, 0.0f);
            break;
        case Direction::NegativeZ:
            rotation = DirectX::XMFLOAT3(M_PI / 2, 0.0f, 0.0f);
            break;
        default:
            throw std::invalid_argument("Invalid direction");
    }

    return rotation;
}


WindowsXpPipesSimulation::Direction WindowsXpPipesSimulation::GetOppositeDirection(const Direction direction) const
{
    switch (direction)
    {
        case Direction::PositiveX:
            return Direction::NegativeX;
        case Direction::NegativeX:
            return Direction::PositiveX;
        case Direction::PositiveY:
            return Direction::NegativeY;
        case Direction::NegativeY:
            return Direction::PositiveY;
        case Direction::PositiveZ:
            return Direction::NegativeZ;
        case Direction::NegativeZ:
            return Direction::PositiveZ;
        default:
            throw std::invalid_argument("Invalid direction");
    }
}

void WindowsXpPipesSimulation::CreatePipeAtCell(const Int3& cellPosition, const Direction direction, GridCell::Type pipeType)
{
    std::unique_ptr<Object3D> pipeObject = nullptr;
    GridCell::Type type = GridCell::EMPTY;
    GridCell::InstanceBufferKey poolKey = GridCell::INVALID;
    int instanceIndex = -1;

    switch (pipeType)
    {
        case GridCell::PIPE_STRAIGHT:
            pipeObject = std::make_unique<Cylinder>(GetCellWorldPosition(cellPosition), GetRotationByDirection(direction), DirectX::XMFLOAT3(0.5f, 1.0f, 0.5f), false);
            type = GridCell::PIPE_STRAIGHT;
            poolKey = GridCell::CYLINDER;
            instanceIndex = _straightCount;
            break;
        case GridCell::PIPE_CORNER:
            pipeObject = std::make_unique<Sphere>(GetCellWorldPosition(cellPosition));
            type = GridCell::PIPE_CORNER;
            poolKey = GridCell::SPHERE;
            instanceIndex = _cornerCount;
            break;
        default:
            return;
    }
    if (!pipeObject)
        return;

    GridCell& gridCell = _grid[cellPosition.x][cellPosition.y][cellPosition.z];
    gridCell.type = type;
    gridCell.poolKey = poolKey;
    gridCell.instanceIndex = instanceIndex;
    gridCell.modelMatrixIndex = _pipeTransforms.size();

    Transform transform = pipeObject->transform;

    _pipeTransforms.push_back(transform);
    if (poolKey == GridCell::CYLINDER)
        _straightCount++;
    else
        _cornerCount++;

    _instanceRenderer.AddInstance(InstanceConstantBuffer(transform.GetWorldMatrix()), poolKey);

}

void WindowsXpPipesSimulation::ExtendCellPipe(GridCell& gridCell, const Direction direction, const float length)
{
    Transform transform = _pipeTransforms[gridCell.modelMatrixIndex];
    float oldLength = transform.scale.y;
    float newLength = oldLength + length;
    transform.scale.y *= (newLength / oldLength);

    switch (direction)
    {
    case Direction::PositiveX:
        transform.position.x += length / 2;
        break;
    case Direction::NegativeX:
        transform.position.x -= length / 2;
        break;
    case Direction::PositiveY:
        transform.position.y += length / 2;
        break;
    case Direction::NegativeY:
        transform.position.y -= length / 2;
        break;
    case Direction::PositiveZ:
        transform.position.z += length / 2;
        break;
    case Direction::NegativeZ:
        transform.position.z -= length / 2;
        break;
    default:
        throw std::invalid_argument("Invalid direction");
    }

    _pipeTransforms[gridCell.modelMatrixIndex] = transform;
    _instanceRenderer.UpdateInstanceData(gridCell.poolKey, gridCell.instanceIndex, InstanceConstantBuffer(transform.GetWorldMatrix()));
}

WindowsXpPipesSimulation::Direction WindowsXpPipesSimulation::GenerateDirection(const std::vector<Direction>& availableDirections, Direction currentDirection, const float turnProbability) const
{
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<> disRandom(0, 1);
    float randomZeroToOne = disRandom(gen);

    if (turnProbability > randomZeroToOne) {
        return GetNextDirection(availableDirections);
    }
    return currentDirection;
}

void WindowsXpPipesSimulation::ResetTurnProbability()
{
    _turnProbability = (float)_shortestStraight / _longestStraight / 2;
}

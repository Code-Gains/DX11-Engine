#include <WindowsXpPipesSimulation.hpp>

WindowsXpPipesSimulation::WindowsXpPipesSimulation(const WRL::ComPtr<ID3D11Device>& device, const Int3& dimensions, const float simulationSpeed):
_device(device),
_dimensions(dimensions),
_simulationSpeed(simulationSpeed)
{
    _timeUntilNextSegment = 1 / simulationSpeed;
    _currentDirection = Direction::PositiveY;

    _grid.resize(dimensions.x);
    for (int x = 0; x < _dimensions.x; ++x)
    {
        _grid[x].resize(dimensions.y);
        for (int y = 0; y < _dimensions.y; ++y)
        {
            _grid[x][y].resize(dimensions.z);
            for (int z = 0; z < _dimensions.z; ++z)
            {
                _grid[x][y][z].type = GridCell::EMPTY;
            }
        }
    }
}

WindowsXpPipesSimulation::~WindowsXpPipesSimulation() {}

bool WindowsXpPipesSimulation::Initialize(ID3D11Device* device)
{
    return true;
}

void WindowsXpPipesSimulation::Update(const float deltaTime)
{
    _timeUntilNextSegment -= deltaTime;
   // std::cout << _timeUntilNextSegment << std::endl;
    if (_timeUntilNextSegment < 0.0f)
    {

        std::vector<Direction> availableDirections = GetAvailableDirections();
        if (std::find(availableDirections.begin(), availableDirections.end(), _currentDirection) == availableDirections.end()) {
            Int3 previousCellPosition = GetPreviousCell(_currentPosition, _currentDirection);
            GridCell previousCell = _grid[previousCellPosition.x][previousCellPosition.y][previousCellPosition.z];

            /*previousCell.pipe->transform.position.y += 0.1;
            previousCell.pipe->transform.scale.y += 0.2;*/

            _currentDirection = GetNextDirection(availableDirections);
            CreatePipeAtCell(_currentPosition, _currentDirection, GridCell::PIPE_CORNER);
            _currentPosition = GetNextCell(_currentPosition);
            _timeUntilNextSegment = 1 / _simulationSpeed;
            return;
        }
        CreatePipeAtCell(_currentPosition, _currentDirection, GridCell::PIPE_STRAIGHT);
        _currentPosition = GetNextCell(_currentPosition);
        _timeUntilNextSegment = 1 / _simulationSpeed;
    }
}

void WindowsXpPipesSimulation::Render(ID3D11DeviceContext* deviceContext, ID3D11Buffer* perObjectConstantBuffer)
{
    for (auto& pipeObject : _pipes)
    {
        pipeObject->Render(deviceContext, perObjectConstantBuffer);
    }
}

Int3 WindowsXpPipesSimulation::GetNextCell(const Int3& currentPosition) const
{
    Int3 nextCell = _currentPosition;

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
         throw std::runtime_error("Simulation is over! No available cells.");
    }

    if (_grid[nextCell.x][nextCell.y][nextCell.z].type != GridCell::EMPTY)
    {
        throw std::runtime_error("Next cell is already occupied");
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
    }

    if (previousCell.x < 0 || previousCell.y < 0 || previousCell.z < 0
        || previousCell.x >= _dimensions.x || previousCell.y >= _dimensions.y || previousCell.z >= _dimensions.z)
    {
        throw std::runtime_error("Previous cell is out of bounds.");
    }

    if (_grid[previousCell.x][previousCell.y][previousCell.z].type == GridCell::EMPTY)
    {
        throw std::runtime_error("Previous cell does not exist.");
    }

    return previousCell;
}

WindowsXpPipesSimulation::Direction WindowsXpPipesSimulation::GetNextDirection(const std::vector<Direction>& availableDirections) const
{
    // Instantiate a random number generator
    std::random_device rd;
    std::mt19937 gen(rd());

    // The range of the distribution is now 0 to the number of available directions minus 1.
    std::uniform_int_distribution<> distrib(0, availableDirections.size() - 1);

    // Generate a random index
    int randomIndex = distrib(gen);

    // Return the direction at the random index
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
            rotation = DirectX::XMFLOAT3(0.0f, 0.0f, PI / 2);
            break;
        case Direction::NegativeX:
            rotation = DirectX::XMFLOAT3(0.0f, 0.0f, -PI / 2);
            break;
        case Direction::PositiveY:
            rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
            break;
        case Direction::NegativeY:
            rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
            break;
        case Direction::PositiveZ:
            rotation = DirectX::XMFLOAT3(-PI / 2, 0.0f, 0.0f);
            break;
        case Direction::NegativeZ:
            rotation = DirectX::XMFLOAT3(PI / 2, 0.0f, 0.0f);
            break;
        default:
            rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
            break;
    }

    return rotation;
}

void WindowsXpPipesSimulation::CreatePipeAtCell(const Int3& cellPosition, const Direction direction, GridCell::Type pipeType)
{
    std::cout << "CreatingPipe: " << (int)direction << std::endl;
    std::shared_ptr<Object3D> pipeObject = nullptr;
    GridCell::Type type = GridCell::EMPTY;
    switch (pipeType)
    {
        case GridCell::PIPE_STRAIGHT:
            pipeObject = std::make_shared<Cylinder>(GetCellWorldPosition(cellPosition), GetRotationByDirection(direction), DirectX::XMFLOAT3(0.5f, 1.0f, 0.5f), false);
            type = GridCell::PIPE_STRAIGHT;
            break;
        case GridCell::PIPE_CORNER:
            pipeObject = std::make_shared<Sphere>(GetCellWorldPosition(cellPosition));
            type = GridCell::PIPE_CORNER;
            break;
        case GridCell::EMPTY:
            break;
        default:
            break;
    }
    if (pipeObject)
        pipeObject->Initialize(_device.Get());
        _pipes.push_back(pipeObject);
        _grid[cellPosition.x][cellPosition.y][cellPosition.z].pipe = pipeObject;
        _grid[cellPosition.x][cellPosition.y][cellPosition.z].type = type;
}
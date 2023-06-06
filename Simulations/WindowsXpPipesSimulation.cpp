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
    // Initialize the grid, the pipe, and the random number generator here.
    // If everything is initialized correctly, return true. Otherwise, return false.
    return true;
}

void WindowsXpPipesSimulation::Update(const float deltaTime)
{
    _timeUntilNextSegment -= deltaTime;
   // std::cout << _timeUntilNextSegment << std::endl;
    if (_timeUntilNextSegment < 0.0f)
    {
        CreatePipeAtCell(_currentPosition, GridCell::PIPE_STRAIGHT);
        _currentPosition = GetNextCell();
        _timeUntilNextSegment = 1 / _simulationSpeed;
    }
}

void WindowsXpPipesSimulation::Render(ID3D11DeviceContext* deviceContext, ID3D11Buffer* perObjectConstantBuffer)
{
    for (int x = 0; x < _dimensions.x; ++x)
    {
        for (int y = 0; y < _dimensions.y; ++y)
        {
            for (int z = 0; z < _dimensions.z; ++z)
            {
                if(_grid[x][y][z].pipe != nullptr)
                    _grid[x][y][z].pipe->Render(deviceContext, perObjectConstantBuffer);
            }
        }
    }
}

std::vector<Object3D*> WindowsXpPipesSimulation::GetAllObjects() {
    std::vector<Object3D*> allObjects = CompositeObject3D::GetAllObjects();

    // Iterate over all grid cells.
    for (const auto& xLayer : _grid) {
        for (const auto& yLayer : xLayer) {
            for (const auto& cell : yLayer) {
                // If the grid cell has a pipe object, add it to the list.
                if (cell.pipe) {
                    allObjects.push_back(cell.pipe.get());
                }
            }
        }
    }

    return allObjects;
}

Int3 WindowsXpPipesSimulation::GetNextCell() const
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
        throw std::out_of_range("Next cell is out of the grid boundaries");
    }

    if (_grid[nextCell.x][nextCell.y][nextCell.z].type != GridCell::EMPTY)
    {
        throw std::runtime_error("Next cell is already occupied");
    }

    return nextCell;
}

WindowsXpPipesSimulation::Direction WindowsXpPipesSimulation::GetNextDirection()
{
    return _currentDirection;
}

DirectX::XMFLOAT3 WindowsXpPipesSimulation::GetCellWorldPosition(const Int3& cellPosition) const
{
    return DirectX::XMFLOAT3(cellPosition.x + transform.position.x, cellPosition.y + transform.position.y, cellPosition.z + transform.position.z);
}

void WindowsXpPipesSimulation::CreatePipeAtCell(const Int3& cellPosition, GridCell::Type pipeType)
{
    std::cout << "CreatingPipe" << std::endl;
    std::unique_ptr<Object3D> pipeObject = nullptr;
    switch (pipeType)
    {
        case GridCell::PIPE_STRAIGHT:
            pipeObject = std::make_unique<Cylinder>(GetCellWorldPosition(cellPosition), false);
            //pipeObject = std::make_unique<Cube>(GetCellWorldPosition(cellPosition));
            break;
        case GridCell::PIPE_CORNER:
            pipeObject = std::make_unique<Sphere>(GetCellWorldPosition(cellPosition));
            break;
        case GridCell::EMPTY:
            break;
        default:
            break;
    }
    if (pipeObject)
        pipeObject->Initialize(_device.Get());
        _grid[cellPosition.x][cellPosition.y][cellPosition.z].pipe = std::move(pipeObject);
}
#pragma once

#include <Object3D.hpp>
#include <IntVectors.hpp>
#include <vector>
#include <memory>
#include <Sphere.hpp>
#include <Cylinder.hpp>
#include <Cube.hpp>
#include <iostream>
#include <Definitions.hpp>
#include <random>
#include <Constants.hpp>
#include <memory>


struct GridCell
{
    enum Type { EMPTY, PIPE_STRAIGHT, PIPE_CORNER };
    Type type;
    std::shared_ptr<Object3D> pipe = nullptr;
};

class WindowsXpPipesSimulation : public Object3D 
{
public:
    enum class Direction {
        PositiveX,
        NegativeX,
        PositiveY,
        NegativeY,
        PositiveZ,
        NegativeZ
    };
private:
    WRL::ComPtr<ID3D11Device> _device = nullptr;
    std::vector<std::vector<std::vector<GridCell>>> _grid;
    std::vector<std::shared_ptr<Object3D>> _pipes;

    Int3 _dimensions;

    Int3 _currentPosition;
    Direction _currentDirection;
    int _currentStraightLength = 0;

    float _simulationSpeed;
    int _shortestStraight;
    int _longestStraight;
    float _turnProbability;
    float _turnProbabilityIncreaseRatio;

    float _timeUntilNextSegment;

public:
    WindowsXpPipesSimulation(const WRL::ComPtr<ID3D11Device>& device, const Int3& dimensions, const float simulationSpeed = 1.0f);
    virtual ~WindowsXpPipesSimulation();

    bool Initialize(ID3D11Device* device) override;
    void Update(const float deltaTime) override;
    void Render(ID3D11DeviceContext* deviceContext, ID3D11Buffer* perObjectConstantBuffer) override;
    void Reset(const Int3& dimensions, const float simulationSpeed);

    GridCell GetCell(const Int3& position) const;
    Int3 GetNextCell(const Int3& currentPosition, const Direction currentDirection) const;
    Int3 GetPreviousCell(const Int3& currentPosition, const Direction direction) const;
    Direction GetNextDirection(const std::vector<Direction>& availableDirections) const;
    Direction GetOppositeDirection(const Direction direction) const;
    std::vector<Direction> GetAvailableDirections() const;
    Direction GenerateDirection(const std::vector<Direction>& availableDirections, Direction currentDirection, const float turnProbability) const;
    DirectX::XMFLOAT3 GetCellWorldPosition(const Int3& cellPosition) const;
    DirectX::XMFLOAT3 GetRotationByDirection(const Direction direction) const;
    void ExtendCellPipe(GridCell& gridCell, const Direction direction, const float length);
    void CreatePipeAtCell(const Int3& cellPosition, const WindowsXpPipesSimulation::Direction direction, GridCell::Type pipeType);
    void ResetTurnProbability();
};
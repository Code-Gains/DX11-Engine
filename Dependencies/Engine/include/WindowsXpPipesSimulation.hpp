#pragma once

#include <Object3D.hpp>
#include <IntVectors.hpp>
#include <vector>
#include <memory>

struct GridCell
{
    enum Type { EMPTY, PIPE_STRAIGHT, PIPE_CORNER };
    Type type;
    std::unique_ptr<Object3D> pipe;
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
    std::vector<std::vector<std::vector<GridCell>>> _grid;

    Int3 _currentPosition;
    Direction _currentDirection;

public:
    WindowsXpPipesSimulation();
    virtual ~WindowsXpPipesSimulation();

    bool Initialize(ID3D11Device* device) override;
    void Update() override;
    void Render(ID3D11DeviceContext* deviceContext) override;

    Int3 GetNextCell() const;
    Direction GetNextDirection();

    void CreatePipeAtCell(const Int3& cellPosition, GridCell::Type pipeType);
};
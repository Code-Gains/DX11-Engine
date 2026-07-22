#pragma once

#include "CameraShotEditorUi.h"
#include "RegistryViewer.h"
#include "System.h"

class AnimationEditor : public System {
public:
    AnimationEditor(entt::registry& registry, RegistryViewer* registryViewerPtr);

    void DrawUi() override;

private:
    RegistryViewer* _registryViewerPtr = nullptr;
    CameraShotEditorUi _cameraShotEditorUi;
};

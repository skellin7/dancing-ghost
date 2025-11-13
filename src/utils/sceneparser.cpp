#include "sceneparser.h"
#include "scenefilereader.h"
#include <glm/gtx/transform.hpp>

#include <chrono>
#include <iostream>

void dfsMakeTree(RenderData &renderData, SceneNode* n, glm::mat4 ctm) {
    for (SceneTransformation *t : n->transformations) {
        if (t->type == TransformationType::TRANSFORMATION_TRANSLATE) {
            ctm = ctm * glm::translate(glm::mat4(1.0f), t->translate);
        } else if (t->type == TransformationType::TRANSFORMATION_ROTATE) {
            ctm = ctm * glm::rotate(glm::mat4(1.0f), t->angle, t->rotate);
        } else if (t->type == TransformationType::TRANSFORMATION_SCALE) {
            ctm = ctm * glm::scale(glm::mat4(1.0f), t->scale);
        } else if (t->type == TransformationType::TRANSFORMATION_MATRIX) {
            ctm = ctm * t->matrix;
        }
    }
    for (ScenePrimitive *p : n->primitives) {
        RenderShapeData d;
        d.ctm = ctm;
        d.primitive = *p;
        renderData.shapes.push_back(d);
    }
    for (SceneLight *l : n->lights) {
        SceneLightData d;
        d.id = l->id;
        d.type = l->type;
        d.color = l->color;
        d.function = l->function;
        d.pos = ctm * glm::vec4(0.f,0.f,0.f,1.f);
        if (l->type != LightType::LIGHT_POINT) {
            d.dir = ctm * l->dir;
        }
        if (l->type == LightType::LIGHT_SPOT) {
            d.penumbra = l->penumbra;
            d.angle = l->angle;
        }
        renderData.lights.push_back((d));
    }
    for (SceneNode *c : n->children) {
        dfsMakeTree(renderData, c, ctm);
    }
}

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readJSON();
    if (!success) {
        return false;
    }

    renderData.globalData = fileReader.getGlobalData();
    renderData.cameraData = fileReader.getCameraData();

    renderData.shapes.clear();
    dfsMakeTree(renderData, fileReader.getRootNode(), glm::mat4(1.0f));

    return true;
}

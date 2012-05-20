#ifndef _NODEATTRIBUTES_H
#define _NODEATTRIBUTES_H

#include "Util.h"
#include <string>
#include <vector>

SCOPED_ENUM(Space)
{
    LOCAL,
    PARENT,
    WORLD
}
END_SCOPED_ENUM()

SCOPED_ENUM(NodeAttributes)
{
    CHILDREN,
    SIZE,
    PHYSICS,
    FILENAME,
    MESH_CONTAINER
}
END_SCOPED_ENUM()

SCOPED_ENUM(NodeFlags)
{
    //ENVIRONMENT = BIT(0), // nodes belong to underlying environment layer
    //ASSET = BIT(1), // nodes belong to asset layer
    //DEVELOPER = BIT(2), // only visible to developer?
    VISIBLE = BIT(3),
    DRAW_BOX = BIT(4), // Should drawing a box involve adding a TEMPORARY layer node to selected node?
    //PAUSED = BIT(5), // blocks logicSelf() and subnode logic()
    LOCKED = BIT(6)
}
END_SCOPED_ENUM()


SCOPED_ENUM(NodeLayer)
{
    // decals, bullets, anything generated at game time that doesn't save along with the map
    // anything temporary or connected to a temporary is never considered part of the saved scene
    TEMPORARY = 0,

    ROOT, // the base node of the scene graph (only 1)

    // environment (base meshes or prefabs that form the environment)
    ENVIRONMENT,
    // child meshes automatically loaded in and managed by a superparent(ENVIRONMENT)'s node
    // attaching to these is a little bit more difficult and if its ever
    // supported, should be done by mesh/node names inside superparent(ENVIRONMENT)'s model file
    SUB_ENVIRONMENT,
    
    DEVELOPER // a reserved layer
}
END_SCOPED_ENUM()

SCOPED_ENUM(NodeType)
{
    NODE,
    ENVIRONMENT,
    ENTITY,
    ACTOR,
    LIGHT,
    PARTICLE_SYSTEM,
    SOUND,
    SELECTION // editor-specific
}
END_SCOPED_ENUM()

const std::vector<std::string> NodeTypeStrings = {
    "node",
    "environment",
    "entity"
    "actor",
    "light",
    "particle_system",
    "sound",
    "selection"
};

#endif


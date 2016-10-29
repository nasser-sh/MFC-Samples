/*   Copyright (c) nasser-sh 2016
 *
 *   Distributed under BSD-style license. See accompanying LICENSE.txt in project
 *   directory.
 */
#pragma once
#include <string>
#include "Mesh.h"


namespace graphics {
namespace mesh_loaders {
    CMesh LoadWavefrontObjFile(std::string const &filename);
}
}


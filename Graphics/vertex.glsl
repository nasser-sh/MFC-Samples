/*   Copyright (c) nasser-sh 2016
 *
 *   Distributed under BSD-style license. See accompanying LICENSE.txt in project
 *   directory.
 */
#version 330


in vec3 position;
out vec4 v_out_color;


void main()
{
    gl_Position = vec4(position, 1.0);
    v_out_color = vec4(0.0, 1.0, 1.0, 1.0);
}
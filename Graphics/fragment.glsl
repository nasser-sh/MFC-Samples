/*   Copyright (c) nasser-sh 2016
 *
 *   Distributed under BSD-style license. See accompanying LICENSE.txt in project
 *   directory.
 */
#version 330


in vec4 v_out_color;
out vec4 f_out_color;


void main()
{
    f_out_color = v_out_color;
}
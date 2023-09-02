#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jsmn.h"

const char *JSON_FILE = "warp.json";
const size_t JSON_FILE_SIZE = 1000;
typedef struct
{
    float x;
    float y;
} point_t;

typedef struct
{
    float x;
    float y;
    float u;
    float v;
    float w;
} warp_vertex_t;

warp_vertex_t convert_point_to_warp_vertex(point_t point, int index)
{
    // convert the points into warp vertices
    // the uv coordinates are either 0 or 1 and are determined by the position of the point in the 2D array
    // the w coordinate is always 1 (for now til we figure out how to use it)
    // 0 -> 0, 0 (top left)
    // 1 -> 1, 0 (top right)
    // 2 -> 1, 1 (bottom right)
    // 3 -> 0, 1 (bottom left)

    warp_vertex_t warp_vertex;
    warp_vertex.x = point.x;
    warp_vertex.y = point.y;
    warp_vertex.w = 1;
    warp_vertex.u = index == 0 || index == 3 ? 0 : 1;
    warp_vertex.v = index > 1 ? 1 : 0;

    return warp_vertex;
}
int main(int argc, char **argv)
{
    printf("Hello, from JSON_Parser!\n");

    // Open JSON_FILE and read contents into a buffer
    FILE *fp;
    char buffer[JSON_FILE_SIZE];
    char temp[JSON_FILE_SIZE];

    fp = fopen(JSON_FILE, "r");
    if (fp == NULL)
    {
        printf("Error opening file %s\n", JSON_FILE);
        exit(1);
    }

    size_t file_size = fread(buffer, sizeof(char), JSON_FILE_SIZE, fp);
    fclose(fp);

    printf("File size: %zu\n", file_size);
    printf("Contents of %s:\n%s\n", JSON_FILE, buffer);
    jsmn_parser parser;
    jsmntok_t tokens[100];

    jsmn_init(&parser);
    int r = jsmn_parse(&parser, buffer, file_size, tokens, 100);

    switch (r)
    {
    case JSMN_ERROR_NOMEM:
        printf("Not enough tokens were provided\n");
        return 1;
        break;
    case JSMN_ERROR_INVAL:
        printf("Invalid character inside JSON string\n");
        return 1;
        break;
    case JSMN_ERROR_PART:
        printf("The string is not a full JSON packet, more bytes expected\n");
        return 1;
        break;
    default:
        printf("Success!\n");
    }

    int num_tokens = r;
    printf("Number of tokens: %d\n", num_tokens);

    for (int i = 0; i < num_tokens; i++)
    {
        printf("Token %d: %.*s\n", i, tokens[i].end - tokens[i].start, buffer + tokens[i].start);
    }

    /*
        Expected JSON Schema:
        Input JSON file is intended to define the points of a warp mesh.
        The mesh is defined by a 2D array of points, where each point is defined by an x and y coordinate on a square grid.
        The JSON file should be structured as follows:
        [
            [x1, y1],
            [x2, y2],
            [x3, y3],
            [x4, y4]
        ]
    */

    // Parse JSON file into a 2D array of points
    point_t points[4]; // 4 points in a warp mesh: top left, top right, bottom right, bottom left
    int point_index = 0;
    int token_index = 1; // Skip the first token, which is the entire JSON file

    while (token_index < num_tokens)
    {
        if (tokens[token_index].type == JSMN_ARRAY)
        {
            // Parse the next two tokens as x and y coordinates
            int x_token_index = token_index + 1;
            int y_token_index = token_index + 2;

            // Convert the x and y coordinates from strings to floats
            char x_buffer[10] = "";
            char y_buffer[10] = "";
            int x_buffer_index = 0;
            int y_buffer_index = 0;

            for (int i = tokens[x_token_index].start; i < tokens[x_token_index].end; i++)
            {
                x_buffer[x_buffer_index] = buffer[i];
                x_buffer_index++;
            }
            for (int i = tokens[y_token_index].start; i < tokens[y_token_index].end; i++)
            {
                y_buffer[y_buffer_index] = buffer[i];
                y_buffer_index++;
            }

            printf("x: %s\n", x_buffer);
            printf("y: %s\n", y_buffer);

            points[point_index].x = atof(x_buffer);
            points[point_index].y = atof(y_buffer);

            point_index++;
            token_index += 3;
        }
        else
        {
            printf("Error: expected array token, got %.*s\n", tokens[token_index].end - tokens[token_index].start, buffer + tokens[token_index].start);
            return 1;
        }
    }

    // Print the points
    printf("Points:\n");
    for (int i = 0; i < 4; i++)
    {
        printf("Point %d: (%f, %f)\n", i, points[i].x, points[i].y);
    }

    // we will need an array of 6 warp vertices, 2 for each triangle in the warp mesh
    warp_vertex_t warp_vertices[6];

    // first triangle
    warp_vertices[0] = convert_point_to_warp_vertex(points[0], 0);
    warp_vertices[1] = convert_point_to_warp_vertex(points[1], 1);
    warp_vertices[2] = convert_point_to_warp_vertex(points[3], 3);

    // second triangle
    warp_vertices[3] = convert_point_to_warp_vertex(points[1], 1);
    warp_vertices[4] = convert_point_to_warp_vertex(points[2], 2);
    warp_vertices[5] = convert_point_to_warp_vertex(points[3], 3);

    // Print the warp vertices

    printf("Warp Vertices:\n");
    for (int i = 0; i < 6; i++)
    {
        printf("Warp Vertex %d: (%f, %f, %f, %f, %f)\n", i, warp_vertices[i].x, warp_vertices[i].y, warp_vertices[i].u, warp_vertices[i].v, warp_vertices[i].w);
    }
    return 0;
}

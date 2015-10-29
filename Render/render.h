#ifndef RENDER_H
#define RENDER_H

#include "common.h"


// void RenderImage(std::string scene_xml, const cv::Vec3d& light_direction);

cv::Vec3d CubeLightDirection(double u, double v, int face_index);
void ParseXml(std::string xml_path, pugi::xml_document& doc);
void DumpXml(const pugi::xml_document& doc, std::string file_path);
void ModifyXmlLight(pugi::xml_document& doc, const cv::Vec3d& light_direction);
void RenderScene(std::string mitsuba_path, std::string xml_path, std::string output_path);
void RenderCubemap(std::string mitsuba_path, std::string scene_path, std::string target_dir, int cubemap_length);
void PrepareDirectory(std::string target_dir);
void StartRendering();
#endif



list(APPEND SOURCE_FILES main.cpp)
list(APPEND SOURCE_FILES Window.cpp Window.h)
list(APPEND SOURCE_FILES GraphicsFramework.cpp GraphicsFramework.h)
list(APPEND SOURCE_FILES Shaders.cpp Shaders.h)
list(APPEND SOURCE_FILES RenderCall.cpp RenderCall.h)
list(APPEND SOURCE_FILES Model.cpp Model.h)
list(APPEND SOURCE_FILES VertexBuffer.cpp VertexBuffer.h)
list(APPEND SOURCE_FILES Vertex.cpp Vertex.h)
list(APPEND SOURCE_FILES ShaderInputMapping.h ShaderInputMapping.cpp)
list(APPEND SOURCE_FILES ImguiHelper.h ImguiHelper.cpp)
list(APPEND SOURCE_FILES ShaderBuffer.h ShaderBuffer.cpp)
list(APPEND SOURCE_FILES MatrixUtils.h MatrixUtils.cpp)
list(APPEND SOURCE_FILES COMObject.h)
list(APPEND SOURCE_FILES StructuredData.h)


add_executable(fisk_model_checker "${SOURCE_FILES}")

set_property(TARGET fisk_model_checker PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

target_link_libraries(fisk_model_checker PUBLIC fisk_tools)
target_link_libraries(fisk_model_checker PUBLIC fisk_imgui)
target_link_libraries(fisk_model_checker PUBLIC d3d11.lib) 
target_link_libraries(fisk_model_checker PUBLIC d3dcompiler.lib) 
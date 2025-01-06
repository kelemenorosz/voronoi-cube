cmake_minimum_required(VERSION 3.20)

add_executable(
	main WIN32
	main.cpp
	application.cpp
	clustering_iterations_render.cpp
	commandqueue.cpp
	cqrender.cpp
	cube_lighting_render.cpp
	pipelinestate.cpp
	resolver.cpp
	resource.cpp
	rootsignature.cpp
	source_reader_callback.cpp
	subspace_render.cpp
	)
add_definitions(-DUNICODE)
set_property(TARGET main PROPERTY CXX_STANDARD 17)
target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/)
install(TARGETS main DESTINATION ${CMAKE_SOURCE_DIR})

add_custom_target(shaders)
set(
	SHADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/cqrender_centroid_pixel.hlsl
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/cqrender_compute.hlsl
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/cqrender_kc3_compute.hlsl
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/cqrender_pixel.hlsl
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/cqrender_point_pixel.hlsl
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/cqrender_point_vertex.hlsl
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/cqrender_vertex.hlsl
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/cube_lighting_render_pixel.hlsl
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/cube_lighting_render_vertex.hlsl
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/ps_clustering_iterations_no_lighting.hlsl
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/ps_clustering_iterations_phong_lighting.hlsl
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/subspace_render_color_normal_pixel.hlsl
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/subspace_render_color_normal_vertex.hlsl
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/subspace_render_constant_red_pixel.hlsl
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/subspace_render_linelist_pixel.hlsl
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/subspace_render_linelist_vertex.hlsl
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/subspace_render_no_color_vertex.hlsl
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/subspace_render_trianglelist_pixel.hlsl
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/subspace_render_trianglelist_vertex.hlsl
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/vs_clustering_iterations_pointlist_no_lighting.hlsl
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/vs_clustering_iterations_trianglelist_phong_lighting.hlsl
)
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/shaders/cqrender_centroid_pixel.hlsl PROPERTIES ShaderType "ps")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/shaders/cqrender_compute.hlsl PROPERTIES ShaderType "cs")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/shaders/cqrender_kc3_compute.hlsl PROPERTIES ShaderType "cs")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/shaders/cqrender_pixel.hlsl PROPERTIES ShaderType "ps")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/shaders/cqrender_point_pixel.hlsl PROPERTIES ShaderType "ps")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/shaders/cqrender_point_vertex.hlsl PROPERTIES ShaderType "vs")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/shaders/cqrender_vertex.hlsl PROPERTIES ShaderType "vs")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/shaders/cube_lighting_render_pixel.hlsl PROPERTIES ShaderType "ps")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/shaders/cube_lighting_render_vertex.hlsl PROPERTIES ShaderType "vs")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/shaders/ps_clustering_iterations_no_lighting.hlsl PROPERTIES ShaderType "ps")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/shaders/ps_clustering_iterations_phong_lighting.hlsl PROPERTIES ShaderType "ps")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/shaders/subspace_render_color_normal_pixel.hlsl PROPERTIES ShaderType "ps")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/shaders/subspace_render_color_normal_vertex.hlsl PROPERTIES ShaderType "vs")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/shaders/subspace_render_constant_red_pixel.hlsl PROPERTIES ShaderType "ps")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/shaders/subspace_render_linelist_pixel.hlsl PROPERTIES ShaderType "ps")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/shaders/subspace_render_linelist_vertex.hlsl PROPERTIES ShaderType "vs")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/shaders/subspace_render_no_color_vertex.hlsl PROPERTIES ShaderType "vs")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/shaders/subspace_render_trianglelist_pixel.hlsl PROPERTIES ShaderType "ps")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/shaders/subspace_render_trianglelist_vertex.hlsl PROPERTIES ShaderType "vs")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/shaders/vs_clustering_iterations_pointlist_no_lighting.hlsl PROPERTIES ShaderType "vs")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/shaders/vs_clustering_iterations_trianglelist_phong_lighting.hlsl PROPERTIES ShaderType "vs")

set_source_files_properties(${SHADER_FILES} PROPERTIES ShaderModel "5_1")
foreach(FILE ${SHADER_FILES})
	get_filename_component(FILE_WE ${FILE} NAME_WE)
	get_source_file_property(shadertype ${FILE} ShaderType)
	get_source_file_property(shadermodel ${FILE} ShaderModel)
	add_custom_command(
		TARGET shaders
		COMMAND fxc.exe /nologo /Emain /T${shadertype}_${shadermodel} /Zi /Fo ${CMAKE_BINARY_DIR}/${FILE_WE}.cso /Fd ${CMAKE_BINARY_DIR}/${FILE_WE}.pdb ${FILE}
		MAIN_DEPENDENCY ${FILE}
		COMMENT "HLSL ${FILE}"
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/shaders
		VERBATIM
	)
endforeach(FILE)

add_dependencies(main shaders)

install(CODE "execute_process(COMMAND \${CMAKE_COMMAND} -E copy \${CMAKE_BINARY_DIR}/cqrender_centroid_pixel.cso \"${CMAKE_SOURCE_DIR}/\")")
install(CODE "execute_process(COMMAND \${CMAKE_COMMAND} -E copy \${CMAKE_BINARY_DIR}/cqrender_compute.cso \"${CMAKE_SOURCE_DIR}/\")")
install(CODE "execute_process(COMMAND \${CMAKE_COMMAND} -E copy \${CMAKE_BINARY_DIR}/cqrender_kc3_compute.cso \"${CMAKE_SOURCE_DIR}/\")")
install(CODE "execute_process(COMMAND \${CMAKE_COMMAND} -E copy \${CMAKE_BINARY_DIR}/cqrender_pixel.cso \"${CMAKE_SOURCE_DIR}/\")")
install(CODE "execute_process(COMMAND \${CMAKE_COMMAND} -E copy \${CMAKE_BINARY_DIR}/cqrender_point_pixel.cso \"${CMAKE_SOURCE_DIR}/\")")
install(CODE "execute_process(COMMAND \${CMAKE_COMMAND} -E copy \${CMAKE_BINARY_DIR}/cqrender_point_vertex.cso \"${CMAKE_SOURCE_DIR}/\")")
install(CODE "execute_process(COMMAND \${CMAKE_COMMAND} -E copy \${CMAKE_BINARY_DIR}/cqrender_vertex.cso \"${CMAKE_SOURCE_DIR}/\")")
install(CODE "execute_process(COMMAND \${CMAKE_COMMAND} -E copy \${CMAKE_BINARY_DIR}/cube_lighting_render_pixel.cso \"${CMAKE_SOURCE_DIR}/\")")
install(CODE "execute_process(COMMAND \${CMAKE_COMMAND} -E copy \${CMAKE_BINARY_DIR}/cube_lighting_render_vertex.cso \"${CMAKE_SOURCE_DIR}/\")")
install(CODE "execute_process(COMMAND \${CMAKE_COMMAND} -E copy \${CMAKE_BINARY_DIR}/ps_clustering_iterations_no_lighting.cso \"${CMAKE_SOURCE_DIR}/\")")
install(CODE "execute_process(COMMAND \${CMAKE_COMMAND} -E copy \${CMAKE_BINARY_DIR}/ps_clustering_iterations_phong_lighting.cso \"${CMAKE_SOURCE_DIR}/\")")
install(CODE "execute_process(COMMAND \${CMAKE_COMMAND} -E copy \${CMAKE_BINARY_DIR}/subspace_render_color_normal_pixel.cso \"${CMAKE_SOURCE_DIR}/\")")
install(CODE "execute_process(COMMAND \${CMAKE_COMMAND} -E copy \${CMAKE_BINARY_DIR}/subspace_render_color_normal_vertex.cso \"${CMAKE_SOURCE_DIR}/\")")
install(CODE "execute_process(COMMAND \${CMAKE_COMMAND} -E copy \${CMAKE_BINARY_DIR}/subspace_render_constant_red_pixel.cso \"${CMAKE_SOURCE_DIR}/\")")
install(CODE "execute_process(COMMAND \${CMAKE_COMMAND} -E copy \${CMAKE_BINARY_DIR}/subspace_render_linelist_pixel.cso \"${CMAKE_SOURCE_DIR}/\")")
install(CODE "execute_process(COMMAND \${CMAKE_COMMAND} -E copy \${CMAKE_BINARY_DIR}/subspace_render_linelist_vertex.cso \"${CMAKE_SOURCE_DIR}/\")")
install(CODE "execute_process(COMMAND \${CMAKE_COMMAND} -E copy \${CMAKE_BINARY_DIR}/subspace_render_no_color_vertex.cso \"${CMAKE_SOURCE_DIR}/\")")
install(CODE "execute_process(COMMAND \${CMAKE_COMMAND} -E copy \${CMAKE_BINARY_DIR}/subspace_render_trianglelist_pixel.cso \"${CMAKE_SOURCE_DIR}/\")")
install(CODE "execute_process(COMMAND \${CMAKE_COMMAND} -E copy \${CMAKE_BINARY_DIR}/subspace_render_trianglelist_vertex.cso \"${CMAKE_SOURCE_DIR}/\")")
install(CODE "execute_process(COMMAND \${CMAKE_COMMAND} -E copy \${CMAKE_BINARY_DIR}/vs_clustering_iterations_pointlist_no_lighting.cso \"${CMAKE_SOURCE_DIR}/\")")
install(CODE "execute_process(COMMAND \${CMAKE_COMMAND} -E copy \${CMAKE_BINARY_DIR}/vs_clustering_iterations_trianglelist_phong_lighting.cso \"${CMAKE_SOURCE_DIR}/\")")
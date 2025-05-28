add_requires("entt", "gtest", "glm >=1.0.1", "glfw >=3.4", "glew", "spdlog", "fmt", "stb", "joltphysics", "miniaudio")

set_project("JoltPhysics-Test")
set_languages("c++20")

add_rules("mode.debug", "mode.release")

includes("../EngineSquared/xmake.lua")

-- add /W4 for windows
if is_plat("windows") then
    add_cxflags("/W4")
end

target("RollingBall")
    set_kind("binary")
    set_default(true)
    add_deps("EngineSquared")

    add_files("src/**.cpp")
    add_includedirs("$(projectdir)/src/")

    add_includedirs("$(projectdir)/src/player")
    add_includedirs("$(projectdir)/src/terrain")
    add_includedirs("$(projectdir)/src/scene")
    add_includedirs("$(projectdir)/src/shader")
    add_includedirs("$(projectdir)/src/utils")
    add_includedirs("$(projectdir)/src/ui")

    add_packages("entt", "glm", "glfw", "glew", "spdlog", "fmt", "stb", "joltphysics", "miniaudio", "rmlui")

    set_rundir("$(projectdir)")


if is_mode("debug") then
    add_defines("ES_DEBUG")
    set_symbols("debug")
    set_optimize("none")
end

if is_mode("release") then
    set_optimize("fastest")
end

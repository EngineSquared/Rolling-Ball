set_project("JoltPhysics-Test")
set_languages("c++20")

add_rules("mode.debug", "mode.release")

add_repositories("package_repo https://github.com/EngineSquared/xrepo.git")

add_requires("enginesquared v0.1.0")

-- add /W4 for windows
if is_plat("windows") then
    add_cxflags("/W4")
end

add_rules("plugin.compile_commands.autoupdate", {outputdir = ".vscode"})
target("RollingBall")
    set_kind("binary")
    set_default(true)
    add_packages("enginesquared")

    add_files("src/**.cpp")
    add_includedirs("$(projectdir)/src/")

    add_includedirs("$(projectdir)/src/player")
    add_includedirs("$(projectdir)/src/terrain")
    add_includedirs("$(projectdir)/src/scene")
    add_includedirs("$(projectdir)/src/shader")
    add_includedirs("$(projectdir)/src/utils")
    add_includedirs("$(projectdir)/src/ui")

    set_rundir("$(projectdir)")


if is_mode("debug") then
    add_defines("ES_DEBUG")
    set_symbols("debug")
    set_optimize("none")
end

if is_mode("release") then
    set_optimize("fastest")
end

const std = @import("std");

pub fn build(b: *std.Build) void {
    //
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});
    const cModule = b.createModule(.{
        .target = target,
        .optimize = optimize,
        .link_libc = true,
    });

    cModule.addCSourceFile(.{
        .file = b.path("src/main.c"),
        .language = .c,
        .flags = &.{ "-gen-cdb-fragment-path", "cdb-frags", "-fcommon" },
    });
    cModule.addCSourceFile(.{
        .file = b.path("src/upng.c"),
        .language = .c,
        .flags = &.{"-fcommon"},
    });
    cModule.addCSourceFile(.{
        .file = b.path("src/textures.c"),
        .language = .c,
        .flags = &.{"-fcommon"},
    });
    cModule.addIncludePath(b.path("src/constants.h"));
    cModule.addIncludePath(b.path("src/textures.h"));
    cModule.addIncludePath(b.path("src/headers"));
    cModule.addIncludePath(.{ .cwd_relative = "" }); //add headers folder
    cModule.addLibraryPath(.{ .cwd_relative = "" }); //add lib folder
    cModule.linkSystemLibrary("SDL2main", .{}); //sdl main magic include
    cModule.linkSystemLibrary("SDL2", .{});

    // cModule.addImport( )

    const exe = b.addExecutable(.{
        .root_module = cModule,
        .name = "raycaster",
    });
    //cModule.linkSystemLibrary("", .{});
    b.installArtifact(exe);
}

// $content = (Get-Content cdb-frags/* -Raw) -join ""
// $content = $content.TrimEnd(",`r`n ")
// "[$content]" | Out-File -Encoding utf8 compile_commands.json

const std = @import("std");

pub fn build(b: *std.Build) !void {
    //
    //const file_step = b.step("File Reader", "read the env file");
    const fileVars = try readEnvFile(b.allocator);

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
    cModule.addIncludePath(.{ .cwd_relative = fileVars.includeFileLocation }); //add headers folder
    cModule.addLibraryPath(.{ .cwd_relative = fileVars.libFileLocation }); //add lib folder
    cModule.linkSystemLibrary("SDL2main", .{}); //sdl main magic include
    cModule.linkSystemLibrary("SDL2", .{});

    const exe = b.addExecutable(.{
        .root_module = cModule,
        .name = "raycaster",
    });
    //cModule.linkSystemLibrary("", .{});
    b.installArtifact(exe);

    //create a run step
    const run_step = b.step("run", "runs the main exe immediately after compilation");
    const run_artifact = b.addRunArtifact(exe);
    run_step.dependOn(&run_artifact.step);
}
const EnvFileVars = struct {
    includeFileLocation: []const u8,
    libFileLocation: []const u8,
};

fn readEnvFile(allocator: std.mem.Allocator) !EnvFileVars {
    //
    var threaded = std.Io.Threaded.init(allocator, .{});
    const io = threaded.io();
    const inMemFile = try std.Io.Dir.cwd().openFile(io, ".env", .{ .mode = .read_only });
    var contents_buff: [1024]u8 = undefined;
    const content_size = try inMemFile.readPositionalAll(io, &contents_buff, 0);
    var file_iter = std.mem.splitScalar(u8, contents_buff[0..content_size], '\n');
    var include_location: ?[]const u8 = null;
    var lib_location: ?[]const u8 = null;
    while (file_iter.next()) |raw_line| {
        //
        const line = std.mem.trimEnd(u8, raw_line, "\r");
        const equals_pos = std.mem.find(u8, line, "=") orelse continue;
        if (std.mem.eql(u8, line[0..equals_pos], "SDL_INCLUDE")) { //will be ignoring the possiblity of empty or non-proper location for now
            //
            include_location = line[equals_pos + 1 ..];
        } else if (std.mem.eql(u8, line[0..equals_pos], "SDL_LIB")) {
            lib_location = line[equals_pos + 1 ..];
        }
    }
    if (include_location == null or lib_location == null) {
        return error.Failed;
    }
    return EnvFileVars{
        .includeFileLocation = try allocator.dupe(u8, include_location.?),
        .libFileLocation = try allocator.dupe(u8, lib_location.?),
    };
}

// $content = (Get-Content cdb-frags/* -Raw) -join ""
// $content = $content.TrimEnd(",`r`n ")
// "[$content]" | Out-File -Encoding utf8 compile_commands.json

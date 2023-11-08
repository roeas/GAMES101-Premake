CurrentWorkingDirectory = os.getcwd()
RootPath = string.sub(CurrentWorkingDirectory, 0, string.len(CurrentWorkingDirectory) - string.len("Script"))
FramePath = path.join(RootPath, "Frame")

AssetPath = path.join(FramePath, "Asset")
SourcePath = path.join(FramePath, "Source")
BinaryPath = path.join(FramePath, "bin")
IntermediatePath = path.join(FramePath, "int")

AssignmentsPath = path.join(SourcePath, "Assignment")
ThirdPartyPath = path.join(SourcePath, "ThirdParty")

print("Root path: "..RootPath)
print("Frame path: "..FramePath)

print("Asset path: "..AssetPath)
print("Source path: "..SourcePath)
print("Binary path: "..BinaryPath)
print("Intermediate path: "..IntermediatePath)

print("Assignment path: "..AssignmentsPath)
print("Thirdparty path: "..ThirdPartyPath)

print("")

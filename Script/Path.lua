CurrentWorkingDirectory = os.getcwd()
RootPath = string.sub(CurrentWorkingDirectory, 0, string.len(CurrentWorkingDirectory) - string.len("Script"))
FramePath = path.join(RootPath, "Frame")

SourcePath = path.join(FramePath, "Source")
AssetPath = path.join(FramePath, "Asset")
BinaryPath = path.join(FramePath, "bin")
IntermediatePath = path.join(FramePath, "int")

AssignmentsPath = path.join(SourcePath, "Assignment")
ThirdPartyPath = path.join(SourcePath, "ThirdParty")

print("Root path: "..RootPath)
print("Frame path: "..FramePath)

print("Source path: "..SourcePath)
print("Asset path: "..AssetPath)
print("Binary path: "..BinaryPath)
print("Intermediate path: "..IntermediatePath)

print("Assignment path: "..AssignmentsPath)
print("Thirdparty path: "..ThirdPartyPath)

print("")

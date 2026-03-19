import sys
import argparse
import subprocess
import shutil
from pathlib import Path

PROJECT_FOLDERS = {
    "pubsub" : "PubSub",
    "dynamic" : "DynamicEventSystem",
    "static" : "StaticEventSystem"
}

def build_project(project: str) -> None:
    folder = PROJECT_FOLDERS[project]
    repo_root = Path(__file__).resolve().parent
    project_dir = repo_root / folder
    build_dir = project_dir / "build"

    if build_dir.exists():
        shutil.rmtree(build_dir)
    build_dir.mkdir(parents=True, exist_ok=True)

    configure_cmd = ["cmake", "-S", str(project_dir), "-B", str(build_dir)]

    # StaticEventSystem runs Python generators at configure time.
    # Pin CMake to the same interpreter used to launch this script.
    if project == "static":
        configure_cmd.append(f"-DPython3_EXECUTABLE={sys.executable}")

    subprocess.run(
        configure_cmd,
        check=True
    )
    subprocess.run(
        ["cmake", "--build", str(build_dir)],
        check=True
    )

def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--project", 
                        help="Project to be built. No arguments will build 'all'.",
                        choices=["pubsub", "dynamic", "static", "all"],
                        default="all")

    args = parser.parse_args()

    print(f"Building '{args.project}'")

    if args.project == "all" or args.project == "":
        for prj in ["pubsub", "dynamic", "static"]:
            build_project(prj)
    else:
        build_project(args.project)
    
    return 0


if __name__ == "__main__":
    sys.exit(main())

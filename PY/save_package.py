from importlib.metadata import distributions
import json
def save_list_installed_packages():
    packages = [dist.metadata['Name'] for dist in distributions()]
    with open("PY/PY_Data/installed_packages.json", "w") as f:
        json.dump({"packages": packages}, f)

def list_installed_packages():
    """List all installed packages in the current environment."""
    packages = [dist.metadata['Name'] for dist in distributions()]
    return packages
if __name__ == "__main__":
    print("Installed packages:", save_list_installed_packages())
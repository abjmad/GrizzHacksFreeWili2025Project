import os

sub_dir = "radio"
os.makedirs(sub_dir, exist_ok=True)

colors = ["red", "blue", "green", "yellow", "white", "off"]
for color in colors:
    with open(os.path.join(sub_dir, f"{color}.sub"), "w") as f:
        f.write(f"FAKE_{color.upper()}_TX\n")

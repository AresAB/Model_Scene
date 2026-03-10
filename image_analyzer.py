import sys
import os

def main():
    image_dir = "results/" + " ".join(sys.argv).split(",")[-1].split("/")[-1]
    counter = "_" + str(len(os.listdir("./results")))

    print(image_dir + counter + ".png")

main()
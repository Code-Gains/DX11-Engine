import numpy as np
from PIL import Image
import argparse
import os

def generate_noise_image(width, height):
    """
    Generate a greyscale image with noise given the width and height.

    Parameters:
    - width: int, the width of the image.
    - height: int, the height of the image.

    Returns:
    - A PIL Image object representing a greyscale image with noise.
    """
    # Generate an array of shape (height, width) with random values
    noise = np.random.rand(height, width) * 255
    
    # Convert the array to uint8
    noise = noise.astype(np.uint8)
    
    # Create the PIL image from the numpy array and convert to greyscale
    image = Image.fromarray(noise, 'L')
    
    return image

def generate_half_white_black_image(width, height):
    """
    Generate an image that is half white and half black given the width and height.

    Parameters:
    - width: int, the width of the image.
    - height: int, the height of the image.

    Returns:
    - A PIL Image object representing an image that is half white and half black.
    """
    # Create an array of shape (height, width) with half the values 0 (black) and half 255 (white)
    half_height = height // 2
    top_half = np.zeros((half_height, width), dtype=np.uint8)
    bottom_half = np.full((height - half_height, width), 255, dtype=np.uint8)
    
    # Combine the top and bottom halves
    combined = np.vstack((top_half, bottom_half))
    
    # Create the PIL image from the numpy array
    image = Image.fromarray(combined, 'L')
    
    return image

# def main():
#     # Create parser for command line arguments
#     parser = argparse.ArgumentParser(description="Generate a greyscale noise image with specified dimensions.")
#     parser.add_argument("width", type=int, help="Width of the image")
#     parser.add_argument("height", type=int, help="Height of the image")

#     # Parse arguments
#     args = parser.parse_args()

#     # Generate noise image
#     img = generate_noise_image(args.width, args.height)
    
#     # Get the directory of the current script
#     script_dir = os.path.dirname(os.path.abspath(__file__))
    
#     # Specify the filename
#     filename = "noise_image.png"
    
#     # Create the full path where the image will be saved
#     save_path = os.path.join(script_dir, filename)
    
#     # Save the image
#     img.save(save_path)
#     print(f"Image saved to {save_path}")

def main():
    # Create parser for command line arguments
    parser = argparse.ArgumentParser(description="Generate a half white, half black image with specified dimensions.")
    parser.add_argument("width", type=int, help="Width of the image")
    parser.add_argument("height", type=int, help="Height of the image")

    # Parse arguments
    args = parser.parse_args()

    # Generate half white and black image
    img = generate_half_white_black_image(args.width, args.height)

    #Get the directory of the current script
    script_dir = os.path.dirname(os.path.abspath(__file__))
    
    # Specify the filename
    filename = "noise_image.png"
    
    # Create the full path where the image will be saved
    save_path = os.path.join(script_dir, filename)
    
    # Save the image
    img.save(save_path)
    print(f"Image saved to {save_path}")


if __name__ == "__main__":
    main()

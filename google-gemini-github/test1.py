"""
At the command line, only need to run once to install the package via pip:

$ pip install google-generativeai
"""

from pathlib import Path
import google.generativeai as genai

genai.configure(api_key="")

# Set up the model
generation_config = {
  "temperature": 0.4,
  "top_p": 1,
  "top_k": 32,
  "max_output_tokens": 4096,
}

safety_settings = [
  {
    "category": "HARM_CATEGORY_HARASSMENT",
    "threshold": "BLOCK_MEDIUM_AND_ABOVE"
  },
  {
    "category": "HARM_CATEGORY_HATE_SPEECH",
    "threshold": "BLOCK_MEDIUM_AND_ABOVE"
  },
  {
    "category": "HARM_CATEGORY_SEXUALLY_EXPLICIT",
    "threshold": "BLOCK_MEDIUM_AND_ABOVE"
  },
  {
    "category": "HARM_CATEGORY_DANGEROUS_CONTENT",
    "threshold": "BLOCK_MEDIUM_AND_ABOVE"
  },
]

model = genai.GenerativeModel(model_name="gemini-1.5-flash",
                              generation_config=generation_config,
                              safety_settings=safety_settings)

# Validate that an image is present
if not (img := Path("beagle.jpeg")).exists():
  raise FileNotFoundError(f"Could not find image: {img}")

image_parts = [
  {
    "mime_type": "image/jpeg",
    "data": Path("beagle.jpeg").read_bytes()
#   },
#   {
#     "mime_type": "image/jpeg",
#     "data": Path("image1.jpeg").read_bytes()
#   },
#   {
#     "mime_type": "image/jpeg",
#     "data": Path("image2.jpeg").read_bytes()
   },
]

prompt_parts = [
#   "top 5 places i can visit in korea",
  image_parts[0],
  "\nwhat is it?\, # This is a photo of a beach in Costa Rica.\n\n",
#   image_parts[1],
#   "\nwhat is it This is a photo of a lily flower.\n\n<start_of_image>Gyeongbokgung Palace\n\nChangdeokgung Palace\n\nBukchon Hanok Village\n\nNamsan Tower\n\n\n",
#   image_parts[2],
#   "\nwhat is it",
]

response = model.generate_content(prompt_parts)
print(response.text)
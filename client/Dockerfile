# Use a Linux base image, e.g., Ubuntu
FROM ubuntu:latest

# Set the working directory
WORKDIR /app

# Set the environment variables
ENV XDG_RUNTIME_DIR=/tmp/runtime-dir
ENV DISPLAY=host.docker.internal:0.0

# Copy your application files into the container
COPY . ./

# Install dependencies and  build your application
RUN apt-get update && apt-get install -y \ 
    g++ \
    libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev libsdl2-gfx-dev \
    && rm -rf /var/lib/apt/lists/* \
    && g++ -g src/*.cpp src/../../global/src/*cpp -Wall -o client.exe -lSDL2 -lSDL2main -lSDL2_ttf -lSDL2_image -lSDL2_gfx

# Specify the command to run your application
CMD ["./client.exe"]

# docker build -t pictionary-client .
# docker run -it --rm pictionary-client
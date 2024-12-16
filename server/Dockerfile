# Use a Linux base image, e.g., Ubuntu
FROM ubuntu:latest

# Set the working directory
WORKDIR /app

# Copy your application files into the container
COPY . ./

# Install dependencies and  build your application
RUN apt-get update && apt-get install -y \ 
    g++ \
    && rm -rf /var/lib/apt/lists/* \
    && g++-13 -g src/*.cpp -Wall -o client.exe

# Specify the command to run your application
CMD ["./client.exe"]

# docker build -t pictionary-client .
# docker run -it --rm pictionary-client
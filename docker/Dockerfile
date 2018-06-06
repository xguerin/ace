# Use an official Python runtime as a parent image
FROM ubuntu:latest

# Set the working directory to /app
WORKDIR /build

# Copy the current directory contents into the container at /app
ADD system /system

# Install any needed packages
RUN bash /system/install.sh

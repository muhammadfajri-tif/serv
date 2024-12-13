# Stage 1: Build stage
FROM gcc:latest AS builder

WORKDIR /app

COPY ./*.c .

COPY ./*.h .

COPY ./Makefile .

# Build the project
RUN mkdir build && \
  make build


# Stage 2: Execution stage
FROM alpine:latest

# Install necessary runtime libraries for Alpine
RUN apk add --no-cache libc6-compat

WORKDIR /app

# Copy static file
COPY ./www /app/www

# Copy the compiled binary from the builder stage
COPY --from=builder /app/server .

EXPOSE 3000

ENV PORT_NUMBER=3000

# Command to run the compiled binary
CMD ["sh", "-c", "./server ${PORT_NUMBER}"]

# Stage 1: Build dependencies
FROM node:20-alpine AS builder
RUN apk add --no-cache git
WORKDIR /app
COPY . .
RUN npm install

# Stage 2: Final image
FROM node:20-alpine
WORKDIR /app
COPY --from=builder /app .
COPY entrypoint.sh .
RUN chmod +x entrypoint.sh
EXPOSE 3000
ENTRYPOINT ["./entrypoint.sh"]
CMD ["npm", "start"]
#!/bin/sh
# Go to app root
cd /app

# Pull latest production branch (optional, can disable if you want to preserve local edits)
git fetch origin
git checkout production
git pull origin production
git clean -fd

# Install dependencies
npm install

# Execute default CMD (npm start)
exec "$@"
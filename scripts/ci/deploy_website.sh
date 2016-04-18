#!/bin/bash

echo "deploying docs for $TRAVIS_BUILD_DIR/docs"

export AWS_ACCESS_KEY_ID="$AWS_KEY"
export AWS_SECRET_ACCESS_KEY="$AWS_SECRET"

docker run -e "AWS_SECRET_ACCESS_KEY=$AWS_SECRET" -e "AWS_ACCESS_KEY_ID=$AWS_KEY" -v $TRAVIS_BUILD_DIR:/data -w /data/doc laszip/docs aws s3 sync ./build/html/ s3://www.liblas.org --acl public-read
docker run -e "AWS_SECRET_ACCESS_KEY=$AWS_SECRET" -e "AWS_ACCESS_KEY_ID=$AWS_KEY" -v $TRAVIS_BUILD_DIR:/data -w /data/doc laszip/docs aws s3 sync ./api/html/ s3://www.liblas.org --acl public-read

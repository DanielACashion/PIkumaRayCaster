const TILE_SIZE = 64;
const MAP_NUM_ROWS = 11;
const MAP_NUM_COLS = 15;
const WINDOW_WIDTH = MAP_NUM_COLS * TILE_SIZE;
const WINDOW_HEIGHT = MAP_NUM_ROWS * TILE_SIZE;
const FOV_ANGLE = 60 * (Math.PI / 180);
const WALL_STRIP_WIDTH = 5;
const NUM_OF_RAYS = WINDOW_WIDTH / WALL_STRIP_WIDTH;



const MINIMAP_SCALE_FACTOR = 0.2;

class Map {
  constructor() {
    this.grid = [
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
      1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
      1, 0, 0, 1, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 1,
      1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 1,
      1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 1,
      1, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 1,
      1, 0, 3, 1, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 1,
      1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 3, 3, 1,
      1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    ];
  }
  render() {
    //
    for (var i = 0; i < MAP_NUM_ROWS * MAP_NUM_COLS; i++) {
      let x = Math.floor(i % MAP_NUM_COLS);
      let y = Math.floor(i / MAP_NUM_COLS);
      var tileX = x * TILE_SIZE;
      var tileY = y * TILE_SIZE;
      var tileColor = this.grid[y * MAP_NUM_COLS + x] != 0 ? "#222" : "#fff";
      stroke("#222");
      fill(tileColor);
      rect(
        MINIMAP_SCALE_FACTOR * tileX,
        MINIMAP_SCALE_FACTOR * tileY,
        MINIMAP_SCALE_FACTOR * TILE_SIZE,
        MINIMAP_SCALE_FACTOR * TILE_SIZE);
    }
  }
  hasWallAt(x, y) {
    if (x < 0 || y < 0 || x > WINDOW_WIDTH || y > WINDOW_HEIGHT) {
      return true;
    }

    let localX = Math.floor(x / TILE_SIZE);
    let localY = Math.floor(y / TILE_SIZE);
    if (this.grid[localY * MAP_NUM_COLS + localX] != 0) {
      return true;
    }
    return false;
  }
  colorAt(x, y) {
    if (x < 0 || y < 0 || x > WINDOW_WIDTH || y > WINDOW_HEIGHT) {
      return 0;
    }
    let localX = Math.floor(x / TILE_SIZE);
    let localY = Math.floor(y / TILE_SIZE);
    if (this.grid[localY * MAP_NUM_COLS + localX] != 0) {
      return this.grid[localY * MAP_NUM_COLS + localX];
    }
    return 0;
  }
}


var grid = new Map();

class Player {
  constructor() {
    this.x = WINDOW_WIDTH * 0.5;
    this.y = WINDOW_HEIGHT * 0.5;
    this.radius = 3;
    this.turnDirection = 0;
    this.walkDirection = 0;
    this.rotationAngle = Math.PI * 0.5;
    this.moveSpeed = 1.0;
    this.rotationSpeed = 1 * (Math.PI / 180);
    this.turnSpeed = 1;
  }
  render() {
    noStroke();
    fill("red");
    circle(
      MINIMAP_SCALE_FACTOR * this.x,
      MINIMAP_SCALE_FACTOR * this.y,
      MINIMAP_SCALE_FACTOR * this.radius);
  }
  update() {
    //todo
    this.rotationAngle += this.turnDirection * this.rotationSpeed;
    let moveStep = this.walkDirection * this.moveSpeed;
    let newX = this.x + Math.cos(this.rotationAngle) * moveStep;
    if (!grid.hasWallAt(newX, this.y)) {
      this.x = newX;
    }
    let newY = this.y + Math.sin(this.rotationAngle) * moveStep;
    if (!grid.hasWallAt(this.x, newY)) {
      this.y = newY;
    }
  }
}

class Ray {
  constructor(rayAngle) {
    this.rayAngle = normalizeAngle(rayAngle);
    this.wallHitX = 0;
    this.wallHitY = 0;
    this.distance = 0;
    this.isRayFacingDown = this.rayAngle > 0 && this.rayAngle < Math.PI;
    this.isRayFacingUp = !this.isRayFacingDown;

    this.isRayFacingRight = this.rayAngle < (0.5 * Math.PI) || this.rayAngle > (1.5 * Math.PI);
    this.isRayFacingLeft = !this.isRayFacingRight;
    this.wasVerticalHit = false;
    this.rayColor = 0;
  }

  cast() {
    //console.log(`facing left: ${this.isRayFacingLeft}. Up: ${this.isRayFacingUp}`)
    ////////////////////////////////////////////
    //horizontal ray grid intersection
    let foundHorzWallHit = false;
    let xstep, ystep;
    let xintercept, yintercept;
    yintercept = Math.floor(player.y / TILE_SIZE) * TILE_SIZE;
    yintercept += this.isRayFacingDown ? TILE_SIZE : 0;

    xintercept = player.x + (yintercept - player.y) / Math.tan(this.rayAngle);

    //calc the increment value for steps
    ystep = TILE_SIZE;
    ystep *= this.isRayFacingUp ? -1 : 1;

    xstep = TILE_SIZE / Math.tan(this.rayAngle);
    xstep *= (this.isRayFacingLeft && xstep > 0) || (this.isRayFacingRight && xstep < 0) ? -1 : 1;

    let nextHorzTouchX = xintercept;
    let nextHorzTouchY = yintercept;

    let horzWallHitX = 0;
    let horzWallHitY = 0;
    let horxzColor = 0;
    while (nextHorzTouchX >= 0 && nextHorzTouchX <= WINDOW_WIDTH &&
      nextHorzTouchY >= 0 && nextHorzTouchY <= WINDOW_HEIGHT) {
      let peeky = nextHorzTouchY;
      if (this.isRayFacingUp) {
        peeky--;
      }
      if (grid.hasWallAt(nextHorzTouchX, peeky)) {
        //
        foundHorzWallHit = true;
        horzWallHitX = nextHorzTouchX;
        horzWallHitY = nextHorzTouchY;
        horxzColor = grid.colorAt(nextHorzTouchX, peeky);
        break;
      }
      nextHorzTouchX += xstep;
      nextHorzTouchY += ystep;
    }

    ////////////////////////////////////////////
    //vertical ray grid intersection
    let foundVertWallHit = false;
    xintercept = Math.floor(player.x / TILE_SIZE) * TILE_SIZE;
    xintercept += this.isRayFacingRight ? TILE_SIZE : 0;

    yintercept = player.y + (xintercept - player.x) * Math.tan(this.rayAngle);

    //calc the increment value for steps
    xstep = TILE_SIZE;
    xstep *= this.isRayFacingLeft ? -1 : 1;

    ystep = TILE_SIZE * Math.tan(this.rayAngle);
    ystep *= (this.isRayFacingUp && ystep > 0) || (this.isRayFacingDown && ystep < 0) ? -1 : 1;

    let nextVertTouchX = xintercept;
    let nextVertTouchY = yintercept;

    let vertWallHitX = 0;
    let vertWallHitY = 0;
    let vertColor = 0;
    while (nextVertTouchX >= 0 && nextVertTouchX <= WINDOW_WIDTH &&
      nextVertTouchY >= 0 && nextVertTouchY <= WINDOW_HEIGHT) {
      let peekx = nextVertTouchX;
      if (this.isRayFacingLeft) {
        peekx -= 1;
      }

      if (grid.hasWallAt(peekx, nextVertTouchY)) {
        //
        foundVertWallHit = true;
        vertWallHitX = nextVertTouchX;
        vertWallHitY = nextVertTouchY;
        vertColor = grid.colorAt(peekx, nextVertTouchY);
        break;
      }
      nextVertTouchX += xstep;
      nextVertTouchY += ystep;
    }
    //calc both distances
    //choose smaller val
    let horzhitdistance = (foundHorzWallHit) ?
      distanceBetweenPoints(player.x, player.y, horzWallHitX, horzWallHitY) : Number.MAX_VALUE;
    let verthitdistance = (foundVertWallHit) ?
      distanceBetweenPoints(player.x, player.y, vertWallHitX, vertWallHitY) : Number.MAX_VALUE;

    if (horzhitdistance < verthitdistance) {
      this.distance = horzhitdistance;
      this.wallHitX = horzWallHitX;
      this.wallHitY = horzWallHitY;
      this.rayColor = horxzColor;
      return;
    }
    this.distance = verthitdistance;
    this.wallHitX = vertWallHitX;
    this.wallHitY = vertWallHitY;
    this.rayColor = vertColor;
    this.wasVerticalHit = true;
  }

  render() {
    stroke("rgba(255, 0, 0, 0.3)");
    line(
      MINIMAP_SCALE_FACTOR * player.x,
      MINIMAP_SCALE_FACTOR * player.y,
      MINIMAP_SCALE_FACTOR * this.wallHitX,
      MINIMAP_SCALE_FACTOR * this.wallHitY
    );
  }
}

var rays = [];
var player = new Player();

function keyPressed() {
  if (keyCode == UP_ARROW || keyCode == 87) {
    player.walkDirection = +player.turnSpeed;
  } else if (keyCode == DOWN_ARROW || keyCode == 83) {
    player.walkDirection = -player.turnSpeed;
  } else if (keyCode == RIGHT_ARROW || keyCode == 68) {
    player.turnDirection += player.turnSpeed;
  } else if (keyCode == LEFT_ARROW || keyCode == 65) {
    player.turnDirection -= player.turnSpeed;
  } else if (keyCode == 16) {
    player.moveSpeed = 2;
  }
}

function keyReleased() {
  if (keyCode == UP_ARROW || keyCode == 87) {
    player.walkDirection = 0;
  } else if (keyCode == DOWN_ARROW || keyCode == 83) {
    player.walkDirection = 0;
  } else if (keyCode == RIGHT_ARROW || keyCode == 68) {
    player.turnDirection -= player.turnSpeed;
  } else if (keyCode == LEFT_ARROW || keyCode == 65) {
    player.turnDirection += player.turnSpeed;
  } else if (keyCode == 16) {
    player.moveSpeed = 1;
  }
}

function unpackColor(colorInt) {
  let r = 0, g = 0, b = 0;
  switch (colorInt) {
    case 1:
      r = 255;
      break;
    case 2:
      g = 255;
      break;
    case 3:
      b = 255;
  }
  return { r: r, g: g, b: b, a: 1 };
}

function render3DProjectedWalls() {
  let i = 0;
  for (let ray of rays) {
    let rayDistance = cos(ray.rayAngle) * ray.distance;
    let distanceProjPlane = (WINDOW_WIDTH * 0.5) / Math.tan(FOV_ANGLE * 0.5);
    let wallStripHeight = (TILE_SIZE / ray.distance) * distanceProjPlane;
    let colorDist = 1 - (ray.distance / (WINDOW_WIDTH * 0.65));
    if (colorDist <= 0){
      continue;
    }
    let { r, g, b, a } = unpackColor(ray.rayColor);
    a *= colorDist;
    fill("rgba(" + r + "," + g + "," + b + "," + a + ")");
    noStroke();
    rect(
      i * WALL_STRIP_WIDTH,
      (WINDOW_HEIGHT * 0.5) - wallStripHeight * 0.5,
      WALL_STRIP_WIDTH,
      wallStripHeight
    );
    i++;
  }

}
function normalizeAngle(angle) {
  angle = angle % (2 * Math.PI);
  if (angle < 0) {
    angle += (2 * Math.PI);
  }
  return angle;
}

function distanceBetweenPoints(x1, y1, x2, y2) {
  return Math.sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

function setup() {
  createCanvas(WINDOW_WIDTH, WINDOW_HEIGHT);
}

function castAllRays() {
  //
  let rayAngle = player.rotationAngle - FOV_ANGLE * 0.5;
  rays = [];
  for (let i = 0; i < NUM_OF_RAYS; i++) {
    let ray = new Ray(rayAngle);
    ray.cast();
    rays.push(ray);
    rayAngle += FOV_ANGLE / NUM_OF_RAYS;
  }
}
function update() {
  //
  player.update();
  castAllRays();
}
function draw() {
  clear("#212121")
  update();
  render3DProjectedWalls();
  grid.render();
  for (let ray of rays) {
    ray.render();
  }

  player.render();
}

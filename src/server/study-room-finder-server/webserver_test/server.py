from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from fastapi.staticfiles import StaticFiles
from fastapi.responses import FileResponse
from pydantic import BaseModel
from typing import Optional
from pathlib import Path
from contextlib import asynccontextmanager
import random
import asyncio


def randomize_rooms():
    """Randomize occupancy and volume for all rooms."""
    for room in all_rooms:
        room.occupancy = max(0, min(room.maxOccupancy, room.occupancy + random.randint(-3, 3)))
        room.volume = max(0, min(100, room.volume + random.randint(-10, 10)))


async def randomize_rooms_task():
    """Background task that randomizes room data every 3 seconds."""
    while True:
        await asyncio.sleep(3)
        randomize_rooms()
        print("Randomized room data")


@asynccontextmanager
async def lifespan(app: FastAPI):
    """Startup/shutdown lifecycle manager."""
    # Start the background task
    task = asyncio.create_task(randomize_rooms_task())
    yield
    # Cancel the task on shutdown
    task.cancel()
    try:
        await task
    except asyncio.CancelledError:
        pass


app = FastAPI(lifespan=lifespan)

# Enable CORS for development (Astro dev server on different port)
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # Allow all origins in dev; restrict in production
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Room data model (mirrors roomData struct from room_data.h)
class RoomData(BaseModel):
    name: str
    occupancy: int = 0
    volume: int = 0
    maxOccupancy: int = 50

# Update request model
class UpdateRequest(BaseModel):
    name: str
    occupancy: Optional[int] = None
    volume: Optional[int] = None

# In-memory room storage (mirrors allRooms array)
all_rooms: list[RoomData] = [
    RoomData(name="Campus Center Lobby", occupancy=73, volume=5, maxOccupancy=50),
    RoomData(name="Van Houten Floor 1", occupancy=1, volume=96, maxOccupancy=30),
    RoomData(name="Van Houten Floor 2", occupancy=1, volume=96, maxOccupancy=30),
    RoomData(name="Van Houten Floor 3", occupancy=58, volume=17, maxOccupancy=40),
    RoomData(name="Van Houten Basement", occupancy=1, volume=96, maxOccupancy=30),
    RoomData(name="Littman Library", occupancy=1, volume=96, maxOccupancy=30),
]

DIST_DIR = Path(__file__).parent.parent / "web" / "dist"


@app.get("/refresh")
async def refresh():
    return [room.model_dump() for room in all_rooms]


@app.get("/")
async def root():
    """Serve index.html for the root path."""
    index_path = DIST_DIR / "index.html"
    if index_path.exists():
        return FileResponse(index_path)
    raise HTTPException(status_code=500, detail="index.html not found")


# Mount static files after explicit routes (serves files from /dist)
# This handles requests like /app.js -> web/dist/app.js
if DIST_DIR.exists():
    app.mount("/", StaticFiles(directory=DIST_DIR, html=True), name="static")


if __name__ == "__main__":
    import uvicorn
    print(f"Serving static files from: {DIST_DIR}")
    print("Server starting at http://localhost:8000")
    uvicorn.run(app, host="0.0.0.0", port=8000)

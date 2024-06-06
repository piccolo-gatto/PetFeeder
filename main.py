import uvicorn
from fastapi import FastAPI
from src.database import engine, Base
from src.users.router import router as users_router
from src.feeders.router import router as feeders_router
from src.collars.router import router as collars_router


server = FastAPI()

from fastapi.middleware.cors import CORSMiddleware

server = FastAPI()
server.add_middleware(
CORSMiddleware,
allow_origins=["*"],
allow_credentials=True,
allow_methods=["*"],
allow_headers=["*"],
)

Base.metadata.create_all(bind=engine)
server.include_router(users_router)
server.include_router(feeders_router)
server.include_router(collars_router)

if __name__ == "__main__":
    uvicorn.run(server, host='0.0.0.0', port=8000)
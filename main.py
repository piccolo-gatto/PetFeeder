import uvicorn
from fastapi import FastAPI
from src.database import engine, Base
from src.users.router import router as users_router


server = FastAPI()

Base.metadata.create_all(bind=engine)
server.include_router(users_router)

if __name__ == "__main__":
    uvicorn.run(server, host='0.0.0.0', port=8000)
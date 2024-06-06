from pydantic import BaseModel


class Feeder(BaseModel):
    user_id: int
    feeder: str

class FeederID(BaseModel):
    feeder: str

class FeederSaved(Feeder):
    id: int
    user_id: int
    feeder: str

    class Config:
        orm_mode = True



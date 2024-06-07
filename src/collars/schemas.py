from pydantic import BaseModel


class Collar(BaseModel):
    feeder_id: int
    collar: str

class CheckCollar(BaseModel):
    feeder: str
    collar: str

class CollarSaved(Collar):
    id: int
    feeder_id: int
    collar: str

    class Config:
        orm_mode = True

class FeederCollar(BaseModel):
    collars: list


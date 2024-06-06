from pydantic import BaseModel


class User(BaseModel):
    user: str


class UserSaved(User):
    id: int
    user: str

    class Config:
        orm_mode = True



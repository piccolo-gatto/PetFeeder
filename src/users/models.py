from sqlalchemy import Column, Integer, String
from src.database import Base


class Users(Base):
    __tablename__ = "users"

    id = Column(Integer, primary_key=True, index=True)
    user = Column(String)



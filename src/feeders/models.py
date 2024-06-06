from sqlalchemy import Column, Integer, String
from src.database import Base 


class Feeders(Base):
    __tablename__ = "feeders"

    id = Column(Integer, primary_key=True, index=True)
    feeder = Column(String)
    user_id = Column(Integer)



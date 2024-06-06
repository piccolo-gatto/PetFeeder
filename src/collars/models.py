from sqlalchemy import Column, Integer, String
from src.database import Base


class Collars(Base):
    __tablename__ = "collars"

    id = Column(Integer, primary_key=True, index=True)
    feeder_id = Column(Integer)
    collar = Column(String)



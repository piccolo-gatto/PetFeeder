from fastapi import APIRouter, Depends
from src.feeders.schemas import Feeder, FeederSaved, FeederID
from src.feeders.models import Feeders
from src.users.schemas import User
from src.users.models import Users
from sqlalchemy.orm import Session
from src.dependencies import get_db

router = APIRouter()


@router.get("/feedes_all")
async def feedes_all(db: Session = Depends(get_db)):
    try:
        feeders = db.query(Feeders).all()
        return feeders
    except Exception as e:
        return {'message': e}


@router.post("/add_feeder", response_model=FeederSaved)
async def add_feeder(request: Feeder, db: Session = Depends(get_db)):
    try:
        data = Feeders(user_id=request.user_id, feeder=request.feeder)
        db.add(data)
        db.commit()
        db.refresh(data)
        return data
    except Exception as e:
        return {'message': e}


@router.post("/feeder_by_user_id")
async def feeder_by_user_id(request: Feeder, db: Session = Depends(get_db)):
    feeders = db.query(Feeders).filter(Feeders.feeder == request.feeder and Feeders.user_id == request.user_id).all()
    try:
        if len(feeders) == 0:
            return {'message': 'None'}
        else:
            return {'message': 'Exist', 'id': feeders[0].id}
    except Exception as e:
        return {'message': e}
    

@router.post("/get_feeder_id")
async def feeder_by_user_id(request: FeederID, db: Session = Depends(get_db)):
    feeders = db.query(Feeders).filter(Feeders.feeder == request.feeder).all()
    try:
        return {'feeder_id': feeders[0].id}
    except Exception as e:
        return {'message': e}
    

@router.post("/feeders_by_user")
async def feeders_by_user(request: User, db: Session = Depends(get_db)):
    user = db.query(Users).filter(Users.user == request.user).all()
    print(user[0])
    feeders = db.query(Feeders).filter(Feeders.user_id == user[0].id).all()
    try:
        res = []
        for feed in feeders:
            res.append(feed.feeder)
        return res
    except Exception as e:
        return {'message': e}
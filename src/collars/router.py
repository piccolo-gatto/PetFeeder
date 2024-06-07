from fastapi import APIRouter, Depends
from src.collars.schemas import Collar, CollarSaved, FeederCollar
from src.collars.models import Collars
from src.feeders.schemas import Feeder, FeederID
from src.feeders.models import Feeders
from sqlalchemy.orm import Session
from src.dependencies import get_db

router = APIRouter()


@router.get("/collars_all")
async def collars_all(db: Session = Depends(get_db)):
    try:
        collars = db.query(Collars).all()
        return collars
    except Exception as e:
        return {'message': e}


@router.post("/add_collar", response_model=CollarSaved)
async def add_collar(request: Collar, db: Session = Depends(get_db)):
    try:
        data = Collars(collar=request.collar, feeder_id = request.feeder_id)
        db.add(data)
        db.commit()
        db.refresh(data)
        return data
    except Exception as e:
        return {'message': e}


@router.post("/collar_by_feeder_id")
async def collar_by_feeder_id(request: Collar, db: Session = Depends(get_db)):
    collars = db.query(Collars).filter(Collars.collar == request.collar and Collars.feeder_id == request.feeder_id).all()
    try:
        if len(collars) == 0:
            return {'message': 'None'}
        else:
            return {'message': 'Exist', 'id': collars[0].id}
    except Exception as e:
        return {'message': e}
    

@router.post("/collars_by_feeder",  response_model=FeederCollar)
async def collars_by_feeder(request: FeederID, db: Session = Depends(get_db)):
    feeder = db.query(Feeders).filter(Feeders.feeder == request.feeder).all()
    print(feeder[0])
    collars = db.query(Collars).filter(Collars.feeder_id == feeder[0].id).all()
    try:
        res = []
        for collar in collars:
            res.append(collar.collar)
        return {'collars': res}
    except Exception as e:
        return {'message': e}
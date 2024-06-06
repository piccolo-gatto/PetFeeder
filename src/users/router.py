from fastapi import APIRouter, Depends
from src.users.schemas import User, UserSaved
from src.users.models import Users
from sqlalchemy.orm import Session
from src.dependencies import get_db

router = APIRouter()


@router.get("/users_all")
async def users_all(db: Session = Depends(get_db)):
    try:
        users = db.query(Users).all()
        return users
    except Exception as e:
        return {'message': e}


@router.post("/add_user", response_model=UserSaved)
async def add_user(request: User, db: Session = Depends(get_db)):
    try:
        data = Users(user=request.user)
        db.add(data)
        db.commit()
        db.refresh(data)
        return data
    except Exception as e:
        return {'message': e}


@router.post("/user_by_chat_id")
async def user_by_chat_id(request: User, db: Session = Depends(get_db)):
    user = db.query(Users).filter(Users.user == request.user).all()
    try:
        if len(user) == 0:
            return {'message': 'None'}
        else:
            return {'message': 'Exist', 'id': user[0].id}
    except Exception as e:
        return {'message': e}
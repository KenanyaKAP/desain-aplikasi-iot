from flask import Flask, make_response
from flask_restful import Api, Resource, reqparse
import pandas as pd
import sqlConnector

app = Flask(__name__)
app.config["DEBUG"] = True
api = Api(app)

class ESP(Resource):
    def get(self):
        data = sqlConnector.getTodayData()
        data = pd.DataFrame(data, columns=["ID","Date","Temp"])
        data["Date"] = data["Date"].dt.strftime("%Y-%m-%d %H:%M:%S")
        data = data.to_dict('records')
        
        # Create the response with CORS headers
        response = make_response({'data': data}, 200)
        response.headers['Access-Control-Allow-Origin'] = '*'
        response.headers['Access-Control-Allow-Credentials'] = 'true'

        return response
    
    def post(self):
        parser = reqparse.RequestParser()
        parser.add_argument('temperature', required=True, type=float, location='args')
        args = parser.parse_args()
        print(args)
        new_data = pd.DataFrame({
            'temperature' : [args['temperature']],
        })
        print(new_data)
        sqlConnector.postData(float(args['temperature']))
        return {'data' : new_data.to_dict('records')}, 201

# Add URL endpoints
api.add_resource(ESP, '/esp')

if __name__ == '__main__':
    app.run(host='0.0.0.0')
# API Documentation - VAT Subsoil Monitor

## 📡 Endpoint Information

**Base URL**: `https://api-vatsubsoil-dev.ggfsystem.com`

**Endpoint**: `/subsoils`

**Method**: `POST`

**Content-Type**: `application/json`

## 📝 Request Format

### Headers
```http
POST /subsoils HTTP/1.1
Host: api-vatsubsoil-dev.ggfsystem.com
Content-Type: application/json
Content-Length: [calculated]
Connection: close
```

### Payload Structure
```json
{
  "type": "sensor",
  "deviceId": "BJK0001",
  "gps": {
    "lat": -6.1234567,
    "lon": 106.1234567,
    "alt": 100.5,
    "sog": 0,
    "cog": 0
  },
  "ultrasonic": {
    "dist1": 25.4,
    "dist2": 23.8
  },
  "timestamp": "2025-01-15T10:30:00"
}
```

## 📋 Field Descriptions

### Root Level
| Field | Type | Description | Required |
|-------|------|-------------|----------|
| `type` | String | Data type identifier ("sensor") | Yes |
| `deviceId` | String | Unique device identifier | Yes |
| `gps` | Object | GPS location data | Yes |
| `ultrasonic` | Object | Sensor distance measurements | Yes |
| `timestamp` | String | ISO 8601 timestamp | Yes |

### GPS Object
| Field | Type | Description | Unit | Range |
|-------|------|-------------|------|-------|
| `lat` | Number | Latitude coordinate | Degrees | -90 to 90 |
| `lon` | Number | Longitude coordinate | Degrees | -180 to 180 |
| `alt` | Number | Altitude above sea level | Meters | 0 to 9000 |
| `sog` | Number | Speed over ground | km/h | 0 to 999 |
| `cog` | Number | Course over ground | Degrees | 0 to 360 |

### Ultrasonic Object
| Field | Type | Description | Unit | Range |
|-------|------|-------------|------|-------|
| `dist1` | Number | Distance from sensor 1 | Centimeters | 0.0 to 999.9 |
| `dist2` | Number | Distance from sensor 2 | Centimeters | 0.0 to 999.9 |

## 📊 Response Format

### Success Response
```http
HTTP/1.1 200 OK
Content-Type: application/json

{
  "status": "success",
  "message": "Data received successfully",
  "id": "generated_data_id",
  "timestamp": "2025-01-15T10:30:01"
}
```

### Error Responses

#### 400 Bad Request
```json
{
  "status": "error",
  "message": "Invalid JSON payload",
  "code": 400
}
```

#### 401 Unauthorized
```json
{
  "status": "error",
  "message": "Device not authorized",
  "code": 401
}
```

#### 422 Unprocessable Entity
```json
{
  "status": "error",
  "message": "Validation failed",
  "errors": [
    {
      "field": "gps.lat",
      "message": "Latitude must be between -90 and 90"
    }
  ],
  "code": 422
}
```

#### 500 Internal Server Error
```json
{
  "status": "error",
  "message": "Internal server error",
  "code": 500
}
```

## 🔐 Authentication

Currently, the API uses device ID-based authentication. Future versions may include:
- API Key authentication
- JWT tokens
- OAuth 2.0

## 📈 Rate Limits

- **Rate Limit**: 100 requests per minute per device
- **Burst Limit**: 10 requests per 10 seconds
- **Daily Limit**: 10,000 requests per day per device

## 🛠️ Data Validation Rules

### GPS Validation
- `lat`: Must be valid latitude (-90 to 90)
- `lon`: Must be valid longitude (-180 to 180)
- `alt`: Must be positive number
- `sog`: Must be non-negative
- `cog`: Must be 0-360 degrees

### Ultrasonic Validation
- `dist1`, `dist2`: Must be positive numbers
- Range: 0.1 to 999.9 cm
- Precision: 1 decimal place

### Timestamp Validation
- Format: ISO 8601 (YYYY-MM-DDTHH:MM:SS)
- Must be within 1 hour of server time
- Timezone: UTC assumed if not specified

## 📝 Example Requests

### Minimal Valid Request
```json
{
  "type": "sensor",
  "deviceId": "BJK0001",
  "gps": {
    "lat": -6.1234567,
    "lon": 106.1234567,
    "alt": 100.0,
    "sog": 0,
    "cog": 0
  },
  "ultrasonic": {
    "dist1": 25.4,
    "dist2": 23.8
  },
  "timestamp": "2025-01-15T10:30:00"
}
```

### Request with Movement Data
```json
{
  "type": "sensor",
  "deviceId": "BJK0001",
  "gps": {
    "lat": -6.1234567,
    "lon": 106.1234567,
    "alt": 100.0,
    "sog": 15.5,
    "cog": 270
  },
  "ultrasonic": {
    "dist1": 22.1,
    "dist2": 24.3
  },
  "timestamp": "2025-01-15T10:30:00"
}
```

## 🔧 Testing

### cURL Example
```bash
curl -X POST https://api-vatsubsoil-dev.ggfsystem.com/subsoils \
  -H "Content-Type: application/json" \
  -d '{
    "type": "sensor",
    "deviceId": "BJK0001",
    "gps": {
      "lat": -6.1234567,
      "lon": 106.1234567,
      "alt": 100.0,
      "sog": 0,
      "cog": 0
    },
    "ultrasonic": {
      "dist1": 25.4,
      "dist2": 23.8
    },
    "timestamp": "2025-01-15T10:30:00"
  }'
```

### Postman Collection
A Postman collection is available for API testing:
- Import the collection file (if available)
- Set environment variables for base URL and device ID
- Run automated tests

## 📊 Monitoring & Analytics

The API provides endpoints for data retrieval (if implemented):

### Get Device Data
```http
GET /subsoils/device/{deviceId}?from={timestamp}&to={timestamp}
```

### Get Latest Data
```http
GET /subsoils/device/{deviceId}/latest
```

## 🚨 Error Handling Best Practices

1. **Retry Logic**: Implement exponential backoff for failed requests
2. **Timeout Handling**: Set reasonable timeout values (30 seconds)
3. **Circuit Breaker**: Stop sending requests after consecutive failures
4. **Local Storage**: Cache data locally when API is unavailable

## 📞 Support

For API-related issues:
- **Email**: api-support@ggfsystem.com
- **Documentation**: [API Docs URL]
- **Status Page**: [Status Page URL]

---

**API Version**: 1.0
**Last Updated**: July 16, 2025

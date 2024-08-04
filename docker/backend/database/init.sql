-- init.sql
-- Create the sequence starting from 100000
CREATE SEQUENCE patient_id_seq START 100000;

-- Set starting user_id from 1000
CREATE SEQUENCE user_id_seq START 1000;

-- Set starting clinic_id from 1000
CREATE SEQUENCE provider_id_seq START 1000;

-- Set starting clinic_id from 1000
CREATE SEQUENCE clinics_id_seq START 1000;

-- Set starting clinic_id from 1000
CREATE SEQUENCE pharmacies_id_seq START 1000;

-- Set starting clinic_id from 1000
CREATE SEQUENCE laboratories_id_seq START 1000;

-- Set starting clinic_id from 1000
CREATE SEQUENCE radiologycenters_id_seq START 1000;


-- Create the table with correct usage of sequence for id column
CREATE TABLE IF NOT EXISTS patients (
    id INTEGER PRIMARY KEY DEFAULT nextval('patient_id_seq'),
    basic_data JSONB,
    health_data JSONB,
    appointments_data JSONB,
    created_on timestamptz DEFAULT current_timestamp,
    updated_on timestamptz DEFAULT current_timestamp
);

-- Create the table with correct usage of sequence for id column
CREATE TABLE users (
    id INTEGER PRIMARY KEY DEFAULT nextval('user_id_seq'),
    username VARCHAR(100) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    role VARCHAR(50) NOT NULL,
    basic_data JSONB
);

-- Create the table with correct usage of sequence for id column
CREATE TABLE providers (
    id INTEGER PRIMARY KEY DEFAULT nextval('provider_id_seq'),
    username VARCHAR(100) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    role VARCHAR(50) NOT NULL,
    basic_data JSONB,
    service_data JSONB
);

-- Create the table with correct usage of sequence for id column
CREATE TABLE clinics (
    id INTEGER PRIMARY KEY DEFAULT nextval('clinics_id_seq'),
    basic_data JSONB,
    providers JSONB
);

-- Create the table with correct usage of sequence for id column
CREATE TABLE pharmacies (
    id INTEGER PRIMARY KEY DEFAULT nextval('pharmacies_id_seq'),
    basic_data JSONB,
    providers JSONB
);

-- Create the table with correct usage of sequence for id column
CREATE TABLE laboratories (
    id INTEGER PRIMARY KEY DEFAULT nextval('laboratories_id_seq'),
    basic_data JSONB,
    providers JSONB
);

-- Create the table with correct usage of sequence for id column
CREATE TABLE radiologycenters (
    id INTEGER PRIMARY KEY DEFAULT nextval('radiologycenters_id_seq'),
    basic_data JSONB,
    providers JSONB
);
-- Create the table user_sessions
CREATE TABLE users_sessions (
    id INT PRIMARY KEY,
    last_login TIMESTAMPTZ,
    last_logout TIMESTAMPTZ
);

-- Create the table user_sessions
CREATE TABLE providers_sessions (
    id INT PRIMARY KEY,
    last_login TIMESTAMPTZ,
    last_logout TIMESTAMPTZ
);

-- Optional: Create a trigger to update `updated_at` on every update
CREATE OR REPLACE FUNCTION update_updated_on_column()
RETURNS TRIGGER AS $$
BEGIN
    NEW.updated_on = current_timestamp;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER patients_table_updated_on
BEFORE UPDATE ON patients
FOR EACH ROW
EXECUTE FUNCTION update_updated_on_column();
PRAGMA foreign_keys=ON;
BEGIN TRANSACTION;
-- Create newDatabase file from bash:
-- $ sqlite3 newDatabase.db < delBassoInitialDb.sql 

-- Create EMPLOYEE table
CREATE TABLE worker(
   workerID  INTEGER PRIMARY KEY AUTOINCREMENT,         -- Primary key
   Nachname                TEXT  NOT NULL, 
   Vorname                 TEXT  NOT NULL,   
   gradingID               INT   NOT NULL,         -- Meister, Monteur
   PersonalNr              INT   NOT NULL UNIQUE,  -- PersonalNr  
   Stundensatz             REAL  NOT NULL,         -- Stundensatz 35.70 €
   Anschrift               CHAR(50),               -- Anschrift
   Gehaltsstufe            INT,                    -- Gehaltsstufe 1, 2, 3
   Wochenstunden           REAL  NOT NULL,         -- Wochenstunden 40h Woche
   FOREIGN KEY(gradingID)   REFERENCES grading(gradingID)
);

CREATE TABLE arch(
   archID INTEGER PRIMARY KEY AUTOINCREMENT, 
   Nummer               INT   NOT NULL UNIQUE,   -- Nummer Architekt
   Name                 TEXT  NOT NULL UNIQUE, 
   Kurzform             TEXT  NOT NULL UNIQUE,
   Anschrift            CHAR(50), 
   Ansprechpartner      TEXT
);

CREATE TABLE sub(
   subID  INTEGER PRIMARY KEY AUTOINCREMENT,
   Nummer               INT   NOT NULL UNIQUE,   -- Nummer subunternehmer
   Name                 TEXT  NOT NULL UNIQUE, 
   Kurzform             TEXT  NOT NULL UNIQUE, 
   Anschrift            CHAR(50)
);

-- Überstunden + Vorschuss
CREATE TABLE runtime(
   runtimeID      INTEGER PRIMARY KEY AUTOINCREMENT,
   workerID       INT      NOT NULL,      -- Mitarbeiter ...
   Wochensumme    REAL     NOT NULL,      -- h / Woche
   Monatssumme    REAL     NOT NULL,      -- h / Monat
   Jahressumme    REAL     NOT NULL,      -- h / Jahr
   Überstunden    REAL     NOT NULL,      -- Jahressaldo
   Vorschuss      REAL     NOT NULL,      -- Bereits geholt
   fehlID         INT      NOT NULL,      -- Fehlzeit ...
   FOREIGN KEY(workerID)   REFERENCES worker(workerID),
   FOREIGN KEY(fehlID)     REFERENCES fehl(fehlID)
);

CREATE TABLE fehlzeit(
   fehlID  INTEGER PRIMARY KEY AUTOINCREMENT,
   Grund            TEXT  NOT NULL,
   Von                  DATE,
   Bis                  DATE
);

CREATE TABLE client(
   clientID INTEGER PRIMARY KEY AUTOINCREMENT, 
   Nummer               INT   NOT NULL UNIQUE,   -- Kundennummer
   Name                 TEXT  NOT NULL UNIQUE, 
   Kurzform             TEXT  NOT NULL UNIQUE, 
   Anschrift            CHAR(50), 
   Abteilung            TEXT, 
   Ansprechpartner      TEXT
);

CREATE TABLE grading(
   gradingID  INTEGER PRIMARY KEY AUTOINCREMENT,
   Einstufung        TEXT  NOT NULL UNIQUE
);

CREATE TABLE prj(
   prjID INTEGER PRIMARY KEY AUTOINCREMENT,         -- Primary key
   clientID          INT      NOT NULL,         -- Kunde ...
   Beschreibung      TEXT     NOT NULL,  -- prjname
   Nummer            INT      NOT NULL UNIQUE,  -- prjnummer 
   Kurzform          TEXT     NOT NULL UNIQUE, 
   subID             INT,                       -- Subunternehmer ...
   archID            INT,                       -- Architekt
   Ansprechpartner   TEXT,
   FOREIGN KEY(clientID)   REFERENCES client(clientID),
   FOREIGN KEY(subID)      REFERENCES sub(subID),
   FOREIGN KEY(archID)     REFERENCES arch(archID)
);

CREATE TABLE worktime(
   worktimID INTEGER PRIMARY KEY AUTOINCREMENT, 
   dat         TEXT  NOT NULL, 
   workerID    INT   NOT NULL,
   prjID       INT   NOT NULL,
   hours       REAL  NOT NULL,
   FOREIGN KEY(workerID)   REFERENCES worker(workerID),
   FOREIGN KEY(prjID)      REFERENCES prj(prjID)
);

COMMIT;

-- ////////////////////////////////////////////////
-- ////////////////////////////////////////////////

-- % Generate worktime test data using matlab
-- %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
-- clearvars('-except',INITIALVARS{:})

-- NN=100;
-- halbe = round(rand(1,NN))*0.5;
-- dat={zeros(NN,1)};
-- lead = 'INSERT INTO "worktime" VALUES (';

-- fd = fopen('/tmp/file','w');

-- for k=1:NN
--    dat{k,1} = sprintf('%s%3i, ''2015-%02i-%02i'', %2g, %2g, %3g);',...
--    lead, k, randi([1 12]), randi([1 31]), randi([1 14]),... 
--    randi([1 7]),randi([1 9])+halbe(k));
--    fprintf(fd, '%s\n', dat{k,1});
-- end

-- fclose(fd);
-- type /tmp/file

-- return

INSERT INTO "grading" (Einstufung) VALUES ('Meister');
INSERT INTO "grading" (Einstufung) VALUES ('Monteur');
INSERT INTO "grading" (Einstufung) VALUES ('Obermonteur');
INSERT INTO "grading" (Einstufung) VALUES ('Vorarbeiter');
INSERT INTO "grading" (Einstufung) VALUES ('Hilfsarbeiter');
INSERT INTO "grading" (Einstufung) VALUES ('Leiharbeiter');
INSERT INTO "grading" (Einstufung) VALUES ('ChefInSpee');
INSERT INTO "grading" (Einstufung) VALUES ('EIB/KNX Prog.');

INSERT INTO "worker" (Nachname, Vorname, PersonalNr, gradingID, Stundensatz, Wochenstunden, Gehaltsstufe, Anschrift) VALUES ('Kreis',       'Torsten',   50, 1, 95.50, 42, NULL, NULL);
INSERT INTO "worker" (Nachname, Vorname, PersonalNr, gradingID, Stundensatz, Wochenstunden, Gehaltsstufe, Anschrift) VALUES ('Schmalong',   'Dirk',      70, 4, 68.00, 42, NULL, NULL);
INSERT INTO "worker" (Nachname, Vorname, PersonalNr, gradingID, Stundensatz, Wochenstunden, Gehaltsstufe, Anschrift) VALUES ('Herb',        'Waldemar',  37, 3, 33.33, 42, NULL, NULL);
INSERT INTO "worker" (Nachname, Vorname, PersonalNr, gradingID, Stundensatz, Wochenstunden, Gehaltsstufe, Anschrift) VALUES ('Campobasso',  'Giuliano',  16, 3, 44.44, 55, NULL, NULL);
INSERT INTO "worker" (Nachname, Vorname, PersonalNr, gradingID, Stundensatz, Wochenstunden, Gehaltsstufe, Anschrift) VALUES ('Sayer',       'Harald',    71, 2, 55.55, 42, NULL, NULL);
INSERT INTO "worker" (Nachname, Vorname, PersonalNr, gradingID, Stundensatz, Wochenstunden, Gehaltsstufe, Anschrift) VALUES ('Biselli',     'Matthias',  12, 2, 66.66, 42, NULL, NULL);
INSERT INTO "worker" (Nachname, Vorname, PersonalNr, gradingID, Stundensatz, Wochenstunden, Gehaltsstufe, Anschrift) VALUES ('Francksen',   'Eike',      29, 5, 77.77, 42, NULL, NULL);
INSERT INTO "worker" (Nachname, Vorname, PersonalNr, gradingID, Stundensatz, Wochenstunden, Gehaltsstufe, Anschrift) VALUES ('Gajicki',     'Vladimir',  34, 2, 88.88, 42, NULL, NULL);
INSERT INTO "worker" (Nachname, Vorname, PersonalNr, gradingID, Stundensatz, Wochenstunden, Gehaltsstufe, Anschrift) VALUES ('Raible',      'Robin',     61, 5, 99.99, 42, NULL, NULL);
INSERT INTO "worker" (Nachname, Vorname, PersonalNr, gradingID, Stundensatz, Wochenstunden, Gehaltsstufe, Anschrift) VALUES ('Zagan',       'Daniel',    92, 2, 54.10, 42, NULL, NULL);
INSERT INTO "worker" (Nachname, Vorname, PersonalNr, gradingID, Stundensatz, Wochenstunden, Gehaltsstufe, Anschrift) VALUES ('Subtirelu',   'Trajan',    102, 2, 52.33, 42, NULL, NULL);
INSERT INTO "worker" (Nachname, Vorname, PersonalNr, gradingID, Stundensatz, Wochenstunden, Gehaltsstufe, Anschrift) VALUES ('Limbasan',    'Eugen',     105, 2, 33.33, 42, NULL, NULL);
INSERT INTO "worker" (Nachname, Vorname, PersonalNr, gradingID, Stundensatz, Wochenstunden, Gehaltsstufe, Anschrift) VALUES ('Roman',       'Mihaj',     60, 2, 90.90, 42, NULL, NULL);
INSERT INTO "worker" (Nachname, Vorname, PersonalNr, gradingID, Stundensatz, Wochenstunden, Gehaltsstufe, Anschrift) VALUES ('Hauschel',    'Rudi',      9,  1, 34.77, 42, NULL, NULL);

INSERT INTO "arch" (Nummer, Name, Kurzform, Anschrift) VALUES (910, 'Aries At GmbH', 'Aries', 'Unter Buchsteig 8, D-78532 Tuttlingen');
INSERT INTO "arch" (Nummer, Name, Kurzform, Anschrift) VALUES (911, 'Architekt Muster A GmbH', 'Architekt A', 'Neben Buchsteig 80, D-78532 Tuttlingen');
INSERT INTO "arch" (Nummer, Name, Kurzform, Anschrift) VALUES (912, 'Architekt Muster B GmbH', 'Architekt B', 'Über Buchsteig 800, D-78532 Tuttlingen');

INSERT INTO "sub" (Nummer, Name, Kurzform, Anschrift) VALUES (810, 'Subunternehmer Max GmbH & Co. KG', 'Sub Max', 'Examplestraße 0, D-78532 Tuttlingen');
INSERT INTO "sub" (Nummer, Name, Kurzform, Anschrift) VALUES (811, 'Subunternehmer Mimi GmbH & Co. KG', 'Sub Mimi', 'Mimistraße 0, D-78532 Tuttlingen');

INSERT INTO "fehlzeit" (Grund, Von, Bis) VALUES ('Krank',  NULL, NULL);
INSERT INTO "fehlzeit" (Grund, Von, Bis) VALUES ('Urlaub', NULL, NULL);
INSERT INTO "fehlzeit" (Grund, Von, Bis) VALUES ('Abfeiern', NULL, NULL);
INSERT INTO "fehlzeit" (Grund, Von, Bis) VALUES ('Urlaub', '2015-10-10', '2015-10-20');

INSERT INTO "client" (Nummer, Name, Kurzform, Anschrift, Abteilung, Ansprechpartner) VALUES (250000, 'Karl STORZ GmbH & Co. KG',            'Storz',    'Mittelstraße 8, D-78532 Tuttlingen',     NULL, NULL);
INSERT INTO "client" (Nummer, Name, Kurzform, Anschrift, Abteilung, Ansprechpartner) VALUES (350000, 'Franz Jakoubek Medizintechnik GmbH',  'Jakoubek', 'Auf der Höhe 15, D-78576 Liptingen',     NULL, NULL);
INSERT INTO "client" (Nummer, Name, Kurzform, Anschrift, Abteilung, Ansprechpartner) VALUES (450000, 'August Wenzler Maschinenbau GmbH',    'Wenzler',  'Eschenwasen 4, D-78549 Spaichingen',     NULL, NULL);
INSERT INTO "client" (Nummer, Name, Kurzform, Anschrift, Abteilung, Ansprechpartner) VALUES (550000, 'Aries AT GmbH',                       'Aries',    'Unter Buchsteig 8, D-78532 Tuttlingen',  NULL, NULL);
INSERT INTO "client" (Nummer, Name, Kurzform, Anschrift, Abteilung, Ansprechpartner) VALUES (999000, 'Diverse Kleinarbeiten',               'Diverse',  NULL,                                     NULL, NULL);

INSERT INTO "prj" (clientID, Beschreibung, Kurzform, Nummer, subID, archID, Ansprechpartner) VALUES (1, 'Schulungszentrum (Dr. Karl-Storz-Straße 34)',                           'DKS-34',                        251000, NULL, NULL, NULL);
INSERT INTO "prj" (clientID, Beschreibung, Kurzform, Nummer, subID, archID, Ansprechpartner) VALUES (1, 'Schulungszentrum (Dr. Karl-Storz-Straße 34), 1.OG',                     'DKS-34, Andalusien',            251100, NULL, NULL, NULL);
INSERT INTO "prj" (clientID, Beschreibung, Kurzform, Nummer, subID, archID, Ansprechpartner) VALUES (1, 'Schulungszentrum (Dr. Karl-Storz-Straße 34), 2.OG',                     'DKS-34, Galicien',              251200, NULL, NULL, NULL);
INSERT INTO "prj" (clientID, Beschreibung, Kurzform, Nummer, subID, archID, Ansprechpartner) VALUES (1, 'Schulungszentrum (Dr. Karl-Storz-Straße 34), 3.OG',                     'DKS-34, Asturien',              251300, NULL, NULL, NULL);
INSERT INTO "prj" (clientID, Beschreibung, Kurzform, Nummer, subID, archID, Ansprechpartner) VALUES (1, 'Schulungszentrum (Dr. Karl-Storz-Straße 34), 3.OG, Nacharbeiten',       'DKS-34, Asturien, Nacharb.',    251400, NULL, NULL, NULL);
INSERT INTO "prj" (clientID, Beschreibung, Kurzform, Nummer, subID, archID, Ansprechpartner) VALUES (1, 'Schulungszentrum (Dr. Karl-Storz-Straße 34), Betriebsmittel VDE Püfung','DKS-34, VDE Geräte',            251099, NULL, NULL, NULL);
INSERT INTO "prj" (clientID, Beschreibung, Kurzform, Nummer, subID, archID, Ansprechpartner) VALUES (1, 'Logistikzentrum (Take-Off Gewerbepark, Aufm Berg 4)',                   'Logistik',                      254000, NULL, NULL, NULL);
INSERT INTO "prj" (clientID, Beschreibung, Kurzform, Nummer, subID, archID, Ansprechpartner) VALUES (1, 'Logistikzentrum (Take-Off Gewerbepark, Aufm Berg 4), Halle 1',          'Logistik, H1',                  254100, NULL, NULL, NULL);
INSERT INTO "prj" (clientID, Beschreibung, Kurzform, Nummer, subID, archID, Ansprechpartner) VALUES (1, 'Logistikzentrum (Take-Off Gewerbepark, Aufm Berg 4), Halle 1, EDV-Anlage', 'Logistik, Andalusien',       254110, NULL, NULL, NULL);
INSERT INTO "prj" (clientID, Beschreibung, Kurzform, Nummer, subID, archID, Ansprechpartner) VALUES (1, 'Logistikzentrum (Take-Off Gewerbepark, Aufm Berg 4), Halle 2',          'Logistik, H2',                  254200, NULL, NULL, NULL);
INSERT INTO "prj" (clientID, Beschreibung, Kurzform, Nummer, subID, archID, Ansprechpartner) VALUES (1, 'Logistikzentrum (Take-Off Gewerbepark, Aufm Berg 4), Halle 2, Hochlager', 'Logistik, Asturien',          254210, NULL, NULL, NULL);
INSERT INTO "prj" (clientID, Beschreibung, Kurzform, Nummer, subID, archID, Ansprechpartner) VALUES (1, 'Logistikzentrum (Take-Off Gewerbepark, Aufm Berg 4), Halle 2, Hochlager, Wartung', 'Logistik, Wartung Asturien', 254219, NULL, NULL, NULL);
INSERT INTO "prj" (clientID, Beschreibung, Kurzform, Nummer, subID, archID, Ansprechpartner) VALUES (2, 'Umbau Halle',                                  'Halle',  351000, NULL, NULL, NULL);
INSERT INTO "prj" (clientID, Beschreibung, Kurzform, Nummer, subID, archID, Ansprechpartner) VALUES (2, 'Wartung, Instandsetzung, VDE, Betr.Mittel',    'Jakubek, Allg.',  351999, NULL, NULL, NULL);
INSERT INTO "prj" (clientID, Beschreibung, Kurzform, Nummer, subID, archID, Ansprechpartner) VALUES (1, 'Wartung, Instandsetzung, VDE, Betr.Mittel',    'Storz, Allg.',    250999, NULL, NULL, NULL);
INSERT INTO "prj" (clientID, Beschreibung, Kurzform, Nummer, subID, archID, Ansprechpartner) VALUES (5, 'Kleinauträge',                                 'Kl.Arb', 999999, NULL, NULL, NULL);
INSERT INTO "prj" (clientID, Beschreibung, Kurzform, Nummer, subID, archID, Ansprechpartner) VALUES (3, 'Reparaturen, Instandsetzung',                  'Rep.',   450999, NULL, NULL, NULL);
INSERT INTO "prj" (clientID, Beschreibung, Kurzform, Nummer, subID, archID, Ansprechpartner) VALUES (1, 'Forschung (Dr. Karl-Storz-Straße 11)',         'DKS-11', 252000, NULL, NULL, NULL);

INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2015-07-02',  8,  5, 5.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2015-04-16',  9,  5, 5.5);INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2011-08-01',  8,  6,   7);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2011-10-29',  5,  6,   3);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2006-02-10',  4,  1, 1.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2010-10-21', 10,  4, 10.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2016-12-14', 12,  3, 6.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2012-11-09',  9,  5,   7);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2009-04-06',  2,  3,   3);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2011-12-01', 10,  7,   9);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2007-12-14', 12,  7,   3);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2012-11-01',  1,  3,   1);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2006-10-10', 11,  2,   7);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2015-10-07',  5,  3, 2.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2009-02-14',  6,  1,   7);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2006-03-21',  8,  1, 3.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2010-02-03',  6,  3, 9.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2011-10-14',  1,  1,   1);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2010-09-05',  4,  5,   3);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2009-02-12', 12,  7, 4.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2013-04-29',  9,  2, 1.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2012-04-03',  3,  1,   7);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2014-10-08', 12,  5, 0.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2009-07-19',  6,  6, 1.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2016-11-11',  9,  3, 0.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2011-10-23', 13,  3, 4.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2007-05-17',  8,  2,   1);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2010-10-06',  7,  1, 9.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2014-07-12',  7,  7, 9.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2011-07-28',  8,  6,   7);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2011-03-19',  3,  2,   9);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2007-08-14',  8,  6, 3.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2006-11-12',  9,  7, 3.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2016-11-24',  9,  4, 4.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2015-08-24', 13,  6, 9.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2011-11-11',  3,  4, 0.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2015-04-21', 11,  5,   7);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2008-06-29',  4,  2,   2);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2014-04-19',  3,  1,   3);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2008-11-29',  1,  5,   0);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2010-07-26',  9,  2, 9.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2014-07-27',  5,  3, 6.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2006-05-06', 11,  6,   7);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2016-10-17',  1,  1,   5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2016-11-29', 13,  3,   3);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2014-03-29', 13,  3, 3.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2013-09-25',  8,  1,   6);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2006-10-20',  8,  2, 7.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2013-05-01', 11,  6,   5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2013-10-10', 12,  5, 3.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2008-11-11',  2,  7, 7.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2016-03-25',  7,  5,   0);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2012-08-15',  1,  4,   0);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2015-08-21',  9,  5,  10);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2007-06-26',  7,  6,   1);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2013-11-05',  6,  1,   2);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2013-11-11',  1,  7,   5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2016-04-21',  1,  4, 3.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2009-12-23',  5,  5,   4);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2012-12-19',  2,  6, 7.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2007-02-09',  6,  4, 3.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2015-10-09',  9,  1, 7.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2009-09-09',  9,  3, 4.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2008-09-25', 13,  4, 2.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2012-03-13', 12,  4, 10.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2010-09-18',  2,  3, 10.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2006-05-08',  3,  3,  10);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2011-10-11',  6,  7, 1.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2006-09-28',  2,  5, 1.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2013-08-02', 11,  7, 4.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2009-01-07',  8,  5, 2.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2013-01-29',  2,  3,   7);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2011-07-12', 10,  2, 7.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2016-04-25', 11,  1, 1.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2010-09-20',  9,  1, 1.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2014-11-23', 13,  7, 2.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2011-10-16',  9,  4,   8);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2006-06-07',  5,  4, 8.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2010-07-04',  3,  3, 4.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2008-04-27',  8,  7,   6);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2008-02-17',  6,  5, 8.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2010-07-07', 13,  4,   3);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2008-12-21', 10,  5,   8);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2012-07-27',  6,  2, 3.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2015-09-26',  5,  5, 0.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2016-06-16', 10,  3,   6);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2009-04-20',  6,  1, 2.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2016-09-20',  5,  1,   6);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2006-04-06',  6,  6,   6);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2009-04-03',  3,  6,   0);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2016-06-03',  8,  6,   3);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2014-09-18',  5,  3,   4);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2010-03-15',  1,  4,   0);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2015-11-11', 13,  7,  10);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2007-01-08',  5,  2,   8);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2008-06-08',  5,  7, 0.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2007-05-26',  3,  7, 10.5);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2010-12-04', 10,  5,   6);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2011-03-05',  9,  6,   7);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2011-12-18',  3,  5,   3);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2014-12-09', 13,  5,   1);
INSERT INTO "worktime" (dat, workerID, prjID, hours) VALUES ('2008-12-15', 12,  6, 0.5);

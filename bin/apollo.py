import os,sys,string
import MySQLdb
import MySQLdb.cursors

_runTable = "run"
_populationAxisTable = "population_axis"
_simulatedPopulationTable = "simulated_population"
_simulatedPopulationAxisValueTable = "simulated_population_axis_value"
_timeSeriesTable = "time_series"

class ApolloSimulatorOutput:
    def __init__(self,runID_):
	self.runID = runID_

	### here are the structures used to define the total output of an Apollo data unit

	### Initiate the database
	self.apolloDB = ApolloDB()
	self.apolloDB.connect()

	### Get the ID from the database for this runID
	SQLStatement = 'SELECT * from '+_runTable+' WHERE label = "' + self.runID + '"'
	self._dbRunIndex = self.apolloDB.query(SQLStatement)[0]['id']
	#print str(self.dbRunIndex)

    def printSimulatedPopulations(self):
	print "Simulated Populations for run: " + str(self.runID)
	for population,id in self.simulatedPopulationsDict.items():
	    print population + ": id = " + str(id)

    def getNewlyInfectedTimeSeriesWithinLocation(self,location):
	tsDict = {}
	fipsTranslate = {}
	### get the populationAxis value for "location"
	LocationAxisID = self.apolloDB._populationAxis()['location']

	### Get distince populations
	SQLStatement = 'select population_id,value from simulated_population_axis_value '\
		      +'where value <> "newly exposed" and population_id in '\
		      +'(select DISTINCT population_id from time_series where run_id = "'\
		      + str(self._dbRunIndex) + '" and population_id in '\
		      +'(select id from (select p.id from simulated_population p, '\
		      +'simulated_population_axis_value v, population_axis a '\
		      +' where p.id = v.population_id and v.value like "newly exposed" '\
		      +'and v.axis_id = a.id and a.label like "disease_state" and '\
		      +'p.label like "% '+location+'%") as x) ) order by population_id'
	popResults = self.apolloDB.query(SQLStatement)
	#print "Pop = " + str(len(popResults))
	for row in popResults:
	    fipsTranslate[row['population_id']] = row['value']
	### Get all of the Results for this time series
	SQLStatement = 'select * from time_series where run_id = "'+str(self._dbRunIndex)+'" '\
		      +'and population_id in (select id from (select p.id '\
		      +'from simulated_population p, simulated_population_axis_value v, '\
		      +'population_axis a where p.id = v.population_id and '\
		      +'v.value like "newly exposed" and v.axis_id = a.id and '\
		      +'a.label like "disease_state" and p.label like "% '+location+'%") as x)'\
		      +' order by population_id, time_step'
	#print SQLStatement
	tsResults = self.apolloDB.query(SQLStatement)
	#print "TS = " + str(len(tsResults))
	for row in tsResults:
	    ## Get the fips code for this population
	    fipsCode = fipsTranslate[row['population_id']]
	    if tsDict.has_key(fipsCode) is False:
		tsDict[fipsCode] = []
	    tsDict[fipsCode].append(row['pop_count'])
	
	return tsDict
					  		   
class ApolloDB:
    
    def __init__(self,host_="warhol-fred.psc.edu",
		 user_="apolloext",dbname_="apollo201"):

	self._host = host_
	self._user = user_
	self._dbname = dbname_
	self._conn = None
	self._DictCursor = None
	self._RegularCursor = None
	self.populationAxis = None
	self.simulatedPopulation = None
	self.simulatedPopulationAxisValue = None

        self.stateToPopulationDict = {'S':'susceptible','E':'exposed','I':'infectious',
                                      'R':'recovered','C':'newly exposed','V':'received vaccine control measure',
                                      'Av':'received antiviral control measure','ScCl':'school that is closed'}
        
        self.stateToDataFileDict = {'S':'susceptible.txt','E':'exposed.txt','I':'infectious.txt',
                                      'R':'recovered.txt','C':'newly_exposed.txt','V':'vacc_administered.txt',
                                      'Av':'av_administered.txt'}

    def connect(self):
	if self._conn is not None:
	    print "Connection to Apollo Database has already been established"
	    return
	try:
	    self._conn = MySQLdb.connect(host=self._host,
					    user=self._user,
					    db=self._dbname)
	    self._conn.autocommit(True)
	except MySQLdb.Error, e:
	    print "Problem connecting to Apollo database: %d %s"%(e.args[0],e.args[1])
            raise e
	
	self._cursor = self._conn.cursor(MySQLdb.cursors.DictCursor)
	self.populationAxis = self._populationAxis()
	self.simulatedPopulation = self._simulatedPopulation()
	
    def close(self):
	if self._conn is None:
	    raise RuntimeError("A connection to the Apollo database"\
			       " has not been made before close is made")

	self._conn.close()

    def query(self,SQLString):
	if self._conn is None:
	    raise RuntimeError("A connection to the Apollo database "\
			       " has not been made before queury is made")
	try:
            self._cursor.execute(SQLString)
        except MySQLdb.Error, e:
            print "Error in Query %s to Apollo: %d: %s"%(SQLString,e.args[0],e.args[1])
            raise e
        rows = self._cursor.fetchall()
	
        return rows

    def insertID(self):
        try:
            return self._conn.insert_id()
        except Exception as e:
            raise e

    def getRunDataDescriptionId(self,label_,source_software=3,destination_software=4,format_="TEXT",type_="SIMULATOR_LOG_FILE"):
        try:
            print "Label = " + str(label_)
            SQLStatement = 'SELECT v.run_data_description_id FROM run_data_description_view v WHERE '\
                'v.format = "%s" AND v.label = "%s" and v.type = "%s" and v.source_software = %s '\
                'and v.destination_software = %s'%(format_,label_,type_,str(source_software),str(destination_software))
            print "SQLStatement = " + SQLStatement
            result = self.query(SQLStatement)
            
            if len(result) > 1:
                raise RuntimeError("getRunDataDescription returned more than one entry "+\
                                    "for (%s,%d,%d,%s,%s)"%(label,source_software,destination_software,format_,type_))
            
            return result[0]["run_data_description_id"]
        except Exception as e:
            raise e
        
    def _populationAxis(self):
        try:
            populationAxisDict = {}
            SQLStatement = "SELECT * from " + _populationAxisTable

            result = self.query(SQLStatement)

            for row in result:
                populationAxisDict[row["label"]] = row["id"]

            return populationAxisDict
        except Exception as e:
            raise e

    def _simulatedPopulation(self):
        try:
            simulatedPopDict = {}
            SQLStatement = "SELECT * from " + _simulatedPopulationTable

            result = self.query(SQLStatement)
            for row in result:
                simulatedPopDict[row["label"]] = row["id"]

            return simulatedPopDict
        except Exception as e:
            raise e

    def checkAndAddSimulatedPopulation(self,diseaseState,location):
        try:
            if self.simulatedPopulation is None:
                raise RuntimeError("Trying to add to Apollo simulated populaton "\
                                   "before populating the simulated population structure")
            if self.populationAxis is None:
                raise RuntimeError("Trying to add to Apollo simulated population "\
                                   "before populating the populationAxis structure")
            ### if this is not already in the database, add it
            populationLabel = diseaseState + " in " + location

            if populationLabel not in self.simulatedPopulation.keys():
                SQLStatement = "INSERT INTO "+_simulatedPopulationTable+\
                               ' SET label = "' + populationLabel + '"'

                self.query(SQLStatement)
                ### update current dictionary
                popID = self.insertID()
                self.simulatedPopulation[populationLabel] = popID

                ### add to the simulated axis value table
                SQLStatement = "INSERT INTO "+_simulatedPopulationAxisValueTable+\
                               ' SET  population_id = "' + str(popID) + '", '\
                               ' axis_id = "' + str(self.populationAxis['disease_state']) + '", '\
                               ' value = "' + str(diseaseState) + '"'
                self.query(SQLStatement)

                SQLStatement = "INSERT INTO "+_simulatedPopulationAxisValueTable+\
                               ' SET  population_id = "' + str(popID)  + '", '\
                               ' axis_id = "' + str(self.populationAxis['location']) + '", '\
                               ' value = "' + str(location) + '"'
                self.query(SQLStatement)

            return self.simulatedPopulation[populationLabel]
        except Exception as e:
            raise e
    
    def getRuns(self):
        try:
            runList = []
            SQLStatement = "SELECT * from " + _runTable
            result = self.query(SQLStatement)
            for row in result:
                runList.append(row['label'])
            return runList
        except Exception as e:
            raise e
    
def main():

    apolloDB = ApolloDB()
    print "Testing connection to Apollo database"
    apolloDB.connect()
    print "Connection to Database successful"
    #apolloDB.populationAxis()
    runList = apolloDB.getRuns()
    print "Closing connection to database"
    apolloDB.close()
    print "Closing successful"

    print "RunList 0: " + runList[0]
    apolloOut = ApolloSimulatorOutput(runList[0])
    apolloOut.getNewlyInfectedTimeSeriesWithinLocation("42003")
    #apolloOut.printSimulatedPopulations()

############
# Main hook
############

if __name__=="__main__":
    main()
	

    

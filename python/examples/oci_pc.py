import liblas
import cx_Oracle
import cx_Oracle as oci

con = oci.Connection('lidar/lidar@ubuntu.hobu.biz/crrel.local')

cur = con.cursor()


def run (operations):
    import copy
    ops = copy.copy(operations)
    
    for op in ops:
        try:
            print op
            cur.execute(op)
        except cx_Oracle.DatabaseError, e:
            print e


def insert_lidar(filename):
    reader = liblas.file.File(filename)

    i= 0

    commit_frequency = 10000
    sql = """INSERT INTO LIDAR_DATA (ID, GEOMETRY) 
                VALUES (:id, 
                        MDSYS.SDO_GEOMETRY(3001, NULL, MDSYS.SDO_POINT_TYPE(:x,:y,:z), NULL, NULL))"""
    rows = []
    for p in reader:

        # sql = "INSERT INTO LIDAR_DATA (ID, GEOMETRY) VALUES (%d,SDO_UTIL.FROM_WKTGEOMETRY('POINT(%.6f %.6f %.6f)'))" % (i, p.x,p.y,p.z)
        # sql = "INSERT INTO LIDAR_DATA (ID, GEOMETRY) VALUES (%d, MDSYS.SDO_GEOMETRY(2001, NULL, MDSYS.SDO_POINT_TYPE(%.6f,%.6f,%.6f), NULL, NULL))" % (i,p.x,p.y,p.z)
    #    sql = "INSERT INTO LIDAR_DATA (ID, GEOMETRY) VALUES (%d, MDSYS.SDO_POINT_TYPE(6,8,10))"%i

        row = { 'x':p.x,
                'y':p.y,
                'z':p.z,
                'id':i}
        rows.append(row)
        if i %commit_frequency == 0:
            cur.executemany(sql, rows)
            con.commit()
            rows = []
            print 'commit'

        i+=1

    cur.executemany(sql,rows)
    con.commit()



cleanup = [  "DELETE FROM BASE",
                "DROP TABLE BASE",
                "DROP TABLE BLKTAB",
                "DROP TABLE INPTAB",
                "DROP TABLE LIDAR_DATA",
                "DELETE FROM USER_SDO_GEOM_METADATA WHERE TABLE_NAME='BASE'",
                "DROP TABLE RESTST",
                ]

run(cleanup)                    

creation = [  'CREATE TABLE LIDAR_DATA (ID NUMBER, GEOMETRY SDO_GEOMETRY)',
                    ]
run(creation)
 
filename = '/Users/hobu/svn/liblas/trunk/test/data/TO_core_last_zoom.las'
insert_lidar(filename)



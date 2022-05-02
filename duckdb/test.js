const { duckdb } = just.library('duckdb', './duckdb.so')


const config = duckdb.createConfig()
duckdb.setConfig(config, 'access_mode', 'READ_WRITE')
duckdb.setConfig(config, 'threads', '8')
duckdb.setConfig(config, 'max_memory', '1GB')
duckdb.setConfig(config, 'default_order', 'DESC')
const db = duckdb.open('foo.duckdb', config)
const con = duckdb.connect(db)
duckdb.query(con, 'CREATE TABLE integers(i INTEGER, j INTEGER);')
duckdb.query(con, 'INSERT INTO integers VALUES (3, 4), (5, 6), (7, NULL);')
const result = duckdb.query(con, 'SELECT * FROM integers')
const rows = duckdb.rowCount(result)
const cols = duckdb.columnCount(result)
for (let row = 0; row < rows; row++) {
  for (let col = 0; col < cols; col++) {
    if (col > 0) just.print(', ', false)
    just.print(duckdb.valueVarchar(result, row, col) || '', false)
  }
  just.print('')
}

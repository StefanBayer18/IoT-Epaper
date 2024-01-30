const Fastify = require("fastify");
const { createPool } = require("mariadb");
const dotenv = require("dotenv");
dotenv.config();

const fastify = Fastify();

const maraidb = createPool({
  user: process.env.DB_USER,
  password: process.env.DB_PASSWORD,
  database: process.env.DB_DATABASE,
  host: process.env.DB_HOST,
  debug: true,
});

/**
 * Execute an SQL statement safely.
 *
 * @param {TemplateStringsArray} strings
 * @param  {...string[]} values
 * @returns
 */
function sql(strings, ...values) {
  return maraidb.query(strings.join("?"), values);
}

fastify.get("/latest", async () => {
  const raw = (await sql`SELECT * FROM weather ORDER BY date DESC LIMIT 1`)[0];
  const date = new Date(raw.date);
  return {
    timestamp: date.getTime(),
    weekday: date.getUTCDay(),
    temperature: raw.temperature,
    humidity: raw.humidity,
  };
});

const port = parseInt(process.env.PORT ?? "8080");
const host = process.env.HOST ?? "localhost";
fastify.listen({ port, host }, (err) => {
  if (err !== null) {
    fastify.log.error(err, "Error starting server");
  }
});

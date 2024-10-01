package routes

import (
    "github.com/gofiber/fiber/v2"
    "github.com/SwampPear/blam/docs/handlers"
)

func SetupRoutes(app *fiber.App) {
	app.Get("/", handlers.Index)
	app.Get("/bloop", handlers.Bloop)
	app.Get("/comments", handlers.Comments)
	app.Get("/data_types", handlers.DataTypes)
	app.Get("/functions", handlers.Functions)
	app.Get("/variables", handlers.Variables)
	app.Get("/control_flow", handlers.ControlFlow)
	app.Use(handlers.Error404)
}